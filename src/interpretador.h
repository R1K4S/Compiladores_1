#ifndef INTERPRETADOR_H
#define INTERPRETADOR_H

#include "ast.h"
#include "ambiente.h"

/* Calcula o valor de uma expressão percorrendo a AST (issues #47 e #48) */
Valor avaliar_expressao(const NoAST* no);

/* Executa um comando: declaração, bloco ou expressão */
void interpretar_comando(const NoAST* no);

/* Cria o ambiente global, executa os comandos do topo do programa e mostra as globais */
void executar_programa(const NoAST* raiz);

#endif