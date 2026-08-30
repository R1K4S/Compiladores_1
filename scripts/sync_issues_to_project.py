#!/usr/bin/env python3
"""
Lê um CSV (colunas: title, body, milestone — labels/assignees são ignorados) e:
  1. Cria cada linha como uma Issue no repositório (evitando duplicar se já existir issue com o mesmo título)
  2. Adiciona a issue ao GitHub Project (v2)
  3. Seta o campo "Sprint" (Iteration) do Project de acordo com a coluna `milestone` do CSV

Usa `gh api graphql` diretamente (em vez de `gh project ...`) porque o subcomando
`gh project` tem um bug conhecido que retorna "unknown owner type" tanto para
token inválido quanto, às vezes, para projetos de conta pessoal (user-owned).
Ver: https://github.com/cli/cli/issues/8885

Autenticação: usa a variável de ambiente GH_TOKEN (o próprio `gh` CLI lê essa env var).
O token precisa ter escopo `repo` + `project`.

Configuração via variáveis de ambiente:
  REPO             -> "owner/repo", ex: "R1K4S/Compiladores_1"
  PROJECT_OWNER    -> dono do Project (login do user ou da org), ex: "R1K4S"
  PROJECT_NUMBER   -> número do Project (aparece na URL, ex: .../projects/3 -> 3)
  SPRINT_FIELD_NAME-> nome do campo Iteration no Project (default: "Sprint")
  CSV_PATH         -> caminho do CSV (default: "issues.csv")
"""

import csv
import json
import os
import subprocess
import sys

REPO = os.environ["REPO"]
PROJECT_OWNER = os.environ["PROJECT_OWNER"]
PROJECT_NUMBER = int(os.environ["PROJECT_NUMBER"])
SPRINT_FIELD_NAME = os.environ.get("SPRINT_FIELD_NAME", "Sprint")
CSV_PATH = os.environ.get("CSV_PATH", "issues.csv")


def run(cmd, **kwargs):
    """Roda um comando e retorna stdout (str), levantando erro claro se falhar."""
    result = subprocess.run(cmd, capture_output=True, text=True, **kwargs)
    if result.returncode != 0:
        print(f"[ERRO] Comando falhou: {' '.join(cmd)}", file=sys.stderr)
        print(result.stderr, file=sys.stderr)
        raise RuntimeError(result.stderr)
    return result.stdout.strip()


def gh_graphql(query, string_vars=None, raw_vars=None):
    """Executa uma query/mutation GraphQL via `gh api graphql` e retorna o `data` já parseado."""
    cmd = ["gh", "api", "graphql", "-f", f"query={query}"]
    for k, v in (string_vars or {}).items():
        cmd += ["-f", f"{k}={v}"]
    for k, v in (raw_vars or {}).items():
        cmd += ["-F", f"{k}={v}"]
    out = run(cmd)
    parsed = json.loads(out)
    if "errors" in parsed:
        raise RuntimeError(f"GraphQL retornou erro: {json.dumps(parsed['errors'], ensure_ascii=False)}")
    return parsed["data"]


def check_auth():
    """Confirma que o token é válido e mostra de quem é, para facilitar diagnóstico nos logs."""
    data = gh_graphql("query { viewer { login } }")
    login = data["viewer"]["login"]
    print(f"Token autenticado como: {login}")


def get_existing_issue_titles():
    out = run([
        "gh", "issue", "list", "--repo", REPO,
        "--state", "all", "--limit", "500",
        "--json", "title,url,number",
    ])
    data = json.loads(out)
    return {item["title"]: item for item in data}


def create_issue(title, body):
    cmd = ["gh", "issue", "create", "--repo", REPO, "--title", title, "--body", body]
    url = run(cmd)
    url = url.strip().splitlines()[-1]
    number = int(url.rstrip("/").split("/")[-1])
    return url, number


def get_issue_node_id(issue_number):
    out = run([
        "gh", "issue", "view", str(issue_number), "--repo", REPO,
        "--json", "id", "--jq", ".id",
    ])
    return out.strip()


PROJECT_QUERY = """
query($login: String!, $number: Int!, $fieldName: String!) {
  user(login: $login) {
    projectV2(number: $number) {
      id
      field(name: $fieldName) {
        ... on ProjectV2IterationField {
          id
          configuration {
            iterations { id title }
            completedIterations { id title }
          }
        }
      }
    }
  }
  organization(login: $login) {
    projectV2(number: $number) {
      id
      field(name: $fieldName) {
        ... on ProjectV2IterationField {
          id
          configuration {
            iterations { id title }
            completedIterations { id title }
          }
        }
      }
    }
  }
}
"""


def get_project_info():
    """Retorna (project_id, sprint_field_id, {iteration_title: iteration_id})."""
    data = gh_graphql(
        PROJECT_QUERY,
        string_vars={"login": PROJECT_OWNER, "fieldName": SPRINT_FIELD_NAME},
        raw_vars={"number": PROJECT_NUMBER},
    )

    project = (data.get("user") or {}).get("projectV2") or (data.get("organization") or {}).get("projectV2")
    if not project:
        raise RuntimeError(
            f"Project número {PROJECT_NUMBER} não encontrado para o owner '{PROJECT_OWNER}' "
            f"(nem como usuário nem como organização). Confirme o número do Project e se o "
            f"token tem acesso a ele (Project > Settings > Manage access)."
        )

    field = project.get("field")
    if not field:
        raise RuntimeError(
            f"Campo '{SPRINT_FIELD_NAME}' não encontrado no Project, ou não é do tipo Iteration."
        )

    iterations = field["configuration"]["iterations"] + field["configuration"]["completedIterations"]
    iteration_map = {it["title"]: it["id"] for it in iterations}

    return project["id"], field["id"], iteration_map


ADD_ITEM_MUTATION = """
mutation($projectId: ID!, $contentId: ID!) {
  addProjectV2ItemById(input: {projectId: $projectId, contentId: $contentId}) {
    item { id }
  }
}
"""


def add_item_to_project(project_id, issue_node_id):
    data = gh_graphql(
        ADD_ITEM_MUTATION,
        string_vars={"projectId": project_id, "contentId": issue_node_id},
    )
    return data["addProjectV2ItemById"]["item"]["id"]


SET_ITERATION_MUTATION = """
mutation($projectId: ID!, $itemId: ID!, $fieldId: ID!, $iterationId: String!) {
  updateProjectV2ItemFieldValue(input: {
    projectId: $projectId,
    itemId: $itemId,
    fieldId: $fieldId,
    value: { iterationId: $iterationId }
  }) {
    projectV2Item { id }
  }
}
"""


def set_sprint(project_id, item_id, sprint_field_id, iteration_id):
    gh_graphql(
        SET_ITERATION_MUTATION,
        string_vars={
            "projectId": project_id,
            "itemId": item_id,
            "fieldId": sprint_field_id,
            "iterationId": iteration_id,
        },
    )


def main():
    print(f"Repo: {REPO} | Project: {PROJECT_OWNER}/{PROJECT_NUMBER} | Campo sprint: {SPRINT_FIELD_NAME}")

    check_auth()

    with open(CSV_PATH, encoding="utf-8") as f:
        rows = list(csv.DictReader(f))
    print(f"{len(rows)} linhas encontradas no CSV.")

    print("Carregando issues já existentes (evita duplicar)...")
    existing_titles = get_existing_issue_titles()

    print("Carregando dados do Project (id, campo Sprint, iterations)...")
    project_id, sprint_field_id, iteration_map = get_project_info()
    print(f"  Iterations encontradas: {list(iteration_map.keys())}")

    created, skipped, linked, sprint_errors = 0, 0, 0, []

    for i, row in enumerate(rows, start=1):
        title = row["title"].strip()
        body = row.get("body", "").strip()
        milestone = row.get("milestone", "").strip()

        print(f"\n[{i}/{len(rows)}] {title}")

        if title in existing_titles:
            print("  já existe, pulando criação da issue")
            issue_number = existing_titles[title]["number"]
            skipped += 1
        else:
            issue_url, issue_number = create_issue(title, body)
            print(f"  criada: {issue_url}")
            created += 1

        issue_node_id = get_issue_node_id(issue_number)
        item_id = add_item_to_project(project_id, issue_node_id)
        linked += 1

        if milestone:
            iteration_id = iteration_map.get(milestone)
            if iteration_id:
                set_sprint(project_id, item_id, sprint_field_id, iteration_id)
                print(f"  sprint definida: {milestone}")
            else:
                msg = f"'{milestone}' não bate com nenhuma iteration do campo '{SPRINT_FIELD_NAME}'"
                print(f"  [aviso] {msg}")
                sprint_errors.append((title, msg))

    print("\n===== Resumo =====")
    print(f"Issues criadas: {created}")
    print(f"Issues já existentes (reaproveitadas): {skipped}")
    print(f"Itens adicionados/atualizados no Project: {linked}")
    if sprint_errors:
        print(f"Avisos de sprint não mapeada ({len(sprint_errors)}):")
        for title, msg in sprint_errors:
            print(f"  - {title}: {msg}")


if __name__ == "__main__":
    main()