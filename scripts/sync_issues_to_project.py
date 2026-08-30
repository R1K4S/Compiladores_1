#!/usr/bin/env python3
"""
Lê um CSV (colunas: title, body, milestone — labels/assignees são ignorados) e:
  1. Cria cada linha como uma Issue no repositório (evitando duplicar se já existir issue com o mesmo título)
  2. Adiciona a issue ao GitHub Project (v2)
  3. Seta o campo "Sprint" (Iteration) do Project de acordo com a coluna `milestone` do CSV

Autenticação: usa a variável de ambiente GH_TOKEN (o próprio `gh` CLI lê essa env var).
O token precisa ter escopo `repo` + `project`.

Configuração via variáveis de ambiente:
  REPO             -> "owner/repo", ex: "R1K4S/minic"
  PROJECT_OWNER    -> dono do Project, ex: "R1K4S"
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
PROJECT_NUMBER = os.environ["PROJECT_NUMBER"]
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


def get_existing_issue_titles():
    out = run([
        "gh", "issue", "list", "--repo", REPO,
        "--state", "all", "--limit", "500",
        "--json", "title,url",
    ])
    data = json.loads(out)
    return {item["title"]: item["url"] for item in data}


def create_issue(title, body):
    cmd = ["gh", "issue", "create", "--repo", REPO, "--title", title, "--body", body]
    url = run(cmd)
    return url.strip().splitlines()[-1]


def get_project_info():
    """Retorna (project_id, sprint_field_id, {iteration_title: iteration_id})."""
    out = run([
        "gh", "project", "view", PROJECT_NUMBER,
        "--owner", PROJECT_OWNER, "--format", "json",
    ])
    project = json.loads(out)
    project_id = project["id"]

    out = run([
        "gh", "project", "field-list", PROJECT_NUMBER,
        "--owner", PROJECT_OWNER, "--format", "json",
    ])
    fields = json.loads(out)["fields"]

    sprint_field = next((f for f in fields if f.get("name") == SPRINT_FIELD_NAME), None)
    if not sprint_field:
        raise RuntimeError(
            f"Campo '{SPRINT_FIELD_NAME}' não encontrado no Project. "
            f"Campos disponíveis: {[f.get('name') for f in fields]}"
        )

    iterations = sprint_field.get("iterations", []) + sprint_field.get("completedIterations", [])
    iteration_map = {it["title"]: it["id"] for it in iterations}

    return project_id, sprint_field["id"], iteration_map


def add_item_to_project(issue_url):
    out = run([
        "gh", "project", "item-add", PROJECT_NUMBER,
        "--owner", PROJECT_OWNER, "--url", issue_url, "--format", "json",
    ])
    return json.loads(out)["id"]


def set_sprint(item_id, project_id, sprint_field_id, iteration_id):
    run([
        "gh", "project", "item-edit",
        "--id", item_id,
        "--field-id", sprint_field_id,
        "--project-id", project_id,
        "--iteration-id", iteration_id,
    ])


def main():
    print(f"Repo: {REPO} | Project: {PROJECT_OWNER}/{PROJECT_NUMBER} | Campo sprint: {SPRINT_FIELD_NAME}")

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
            issue_url = existing_titles[title]
            skipped += 1
        else:
            issue_url = create_issue(title, body)
            print(f"  criada: {issue_url}")
            created += 1

        item_id = add_item_to_project(issue_url)
        linked += 1

        if milestone:
            iteration_id = iteration_map.get(milestone)
            if iteration_id:
                set_sprint(item_id, project_id, sprint_field_id, iteration_id)
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