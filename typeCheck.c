#include <stdio.h>
#include <stdlib.h>
#include "sys/queue.h"
#include "tree.h"
#include "y.tab.h"
#include "symbolTable.h"

extern int top;

int currentScope = 0;
int recordDepth = 0;
int identCounter = 0;
struct NodeST* SThead;

void prST (void){
	int i, k;
	printf("\nSYMBOL TABLE - SCOPE %d\n", currentScope);
	for (i = 0; i <= top; i++){
		int t1 = SThead->data[i].type;
		char t1C[100];
		sprintf(t1C, "%d", t1);
		if(SThead->data[i].index != 0){
			printf ("%3d %-10s\t%s\n", i, id_name(i), t1 == IntConst ? "Integer" : t1 == Boolean ? "Boolean" : t1 == RealConst ? "Real" : t1 == Record ? "Record" : t1C);
			for(k = 1; k <= top; k++){
				int t2 = SThead->data[i].fields[k].type;
				char t2C[100];
				sprintf(t2C, "%d", t2);
				if(SThead->data[i].fields[k].index != 0){
					printf("%4d.%d%-10s\t%s\n", i, k, id_name(i), id_name(k), t2 == IntConst ? "Integer" : t2 == Boolean ? "Boolean" : t2 == RealConst ? "Real" : t2 == Record ? "Record" : t2C);
				}
			}
			identCounter++;
		}
	}
	if(identCounter == 0){
		printf("No identifiers declared in this scope.");
	}
	printf("\n");
}

static int check_expr(tree t){
	if (t == NULL) { 
		fprintf (stderr, "Error: missing expression\n");
		return NoType;
	}
	switch (t->kind) {
		case Eq : case Ne :
		case Lt : case Le :
		case Gt : case Ge : {
			int t1 = check_expr (t->first), t2 = check_expr (t->second);
			if (t1 != t2) {
				if(t1 == RealConst && t2 == IntConst){
					return (t->value = Boolean);
				} else if(t1 == IntConst && t2 == RealConst){
					return (t->value = Boolean);
				} else {
					fprintf (stderr, "Error: Type mismatch in comparison\n");
					return (t->value = NoType);
				}
			} else {
				return (t->value = Boolean);
			}
			break;
		}
		case Plus : case Minus : {
			int t1 = check_expr (t->first);
			if(t->second != NULL){
				int t2 = check_expr (t->second);
				if (t1 != IntConst || t2 != IntConst) {
					if(t1 == RealConst && t2 == RealConst) return RealConst;
					if(t1 == RealConst && t2 == IntConst) return RealConst;
					if(t1 == IntConst && t2 == RealConst) return RealConst;
					fprintf (stderr, "Error: Type mismatch in binary arithmetic expression %d\n", t->kind);
					return (t->value = NoType);
				} else {
					return (t->value = IntConst);
				}
			} else {
				if(t1 == IntConst) return IntConst;
				else if(t1 == RealConst) return RealConst;
					fprintf(stderr, "Error: Type mismatch on unary expression\n");
					return (t->value = NoType);
				}
			break;
		}
		case Mult : case Mod : {
			int t1 = check_expr(t->first);
			int t2 = check_expr(t->second);
			if(t1 == IntConst && t2 == IntConst){
				return (t->value = IntConst);
			} else if(t1 == RealConst && t2 == RealConst){
				return (t->value = RealConst);
			} else if((t1 == IntConst && t2 == RealConst) || (t1 == RealConst && t2 == IntConst)){
				return (t->value = RealConst);
			} else {
				fprintf(stderr, "Error: Type mismatch in binary arithmetic expression\n");
				return (t->value = NoType);
			}
			break;
		}
		case Slash : {
			int t1 = check_expr(t->first), t2 = check_expr(t->second);
			if((t1 == RealConst || t1 == IntConst) && (t2 == RealConst || t2 == IntConst)){
				return (t->value = RealConst);
			} else {
				fprintf(stderr, "Error: Type mismatch in '\\' division\n");
				return (t->value = NoType);
			}
			break;
		}
		case Div : {
                        int t1 = check_expr(t->first), t2 = check_expr(t->second);
			if((t1 == RealConst || t1 == IntConst) && (t2 == RealConst || t2 == IntConst)){
				return (t->value = IntConst);
			} else {
				fprintf(stderr, "Error: Type mismatch in 'div' division\n");
                                return (t->value = NoType);
			}	
			break;	
		}
		case And : case Or :
		case Not : {
			int t1 = check_expr(t->first);
			int t2 = check_expr(t->second);
			if(t1 == Boolean && t2 == Boolean){
				return (t->value = Boolean);
			} else {
				fprintf(stderr, "Error: Non-boolean argument in boolean comparison\n");
				return (t->value = NoType);
			}
			break;
		}
		case Dot : {
			int p1 = t->first->value;
			if(SThead->data[p1].index == 0){
				struct NodeST* temp = SThead;
				while(temp->data[p1].index == 0){
					temp = temp->next;
					if(temp == NULL){
						fprintf(stderr, "Error: No record %s within scope\n", id_name(p1));
						return(t->value = NoType);
					}
				}
			}
			if(SThead->data[p1].type == Record){
				int p2 = t->second->value;
				if(SThead->data[p1].fields[p2].index == 0){
					fprintf(stderr, "Error: %s is not a property of record %s\n", id_name(p2), id_name(p1));
					return (t->value = NoType);
				} else {
					return (SThead->data[p1].fields[p2].type);
				}
			} else {
				fprintf(stderr, "Error: Dot operator used on non-record\n");
				return (t->value = NoType);
			}
			break;
		}
		case Ident : {
			int pos = t->value;
			struct NodeST* temp = SThead;
			if (SThead->data[pos].index == 0){
				while(temp->data[pos].index == 0){
					temp = temp->next;
					if(temp == NULL){
						fprintf(stderr, "Error: identifier %s not found within scope\n", id_name(pos));
						return (t->value = NoType);
					}
				}
				return (t->value = temp->data[pos].type);
			}
			return (t->value = SThead->data[pos].type);
			break;
		}
		case IntConst :
			return (t->value = IntConst);
			break;
		case RealConst :
			return (t->value = RealConst);
			break;
		default:
			fprintf (stderr, "Error: invalid expression operator\n");
	}
}

void check(tree t){
	for (t; t != NULL; t = t->next){
		switch (t->kind) {
			case Prog : {
				int i, k;
				STentry newTable[100];
				push(&SThead, newTable);
				for(i = 0; i < 100; i++){
					SThead->data[i].index = 0;
					SThead->data[i].type = NoType;
					for(k = 0; k < 100; k++){
						SThead->data[i].fields[k].index = 0;
						SThead->data[i].fields[k].type = NoType;
					}
				}
				check(t->first);
				printf ("\n");
				prST ();
				break;
			}
			case Assign : {
				int t1 = check_expr(t->first);
				int t2 = check_expr(t->second);
				if((t1 == IntConst && t2 == RealConst) || (t1 == RealConst && t2 == IntConst) || (t1 == t2)){
					
				} else {
					fprintf(stderr, "Error: Type mismatch in identifier assignment\n");
				}
				int pos = t->first->value;
				if(SThead->data[pos].index != 0)
					fprintf(stderr, "Error: Reassignment of identifier %s\n", id_name(t->first->value));
				break;
			}
			case Colon : {
				if(t->first->kind == Var){
					tree varTree = t->first;
					tree idList  = varTree->first;
					for(; idList != NULL; idList = idList->next){
						int pos = idList->value;
						if(SThead->data[pos].index == 0){
							SThead->data[pos].index = pos;
							if(t->second->kind == Int){
								SThead->data[pos].type = IntConst;
							} else if(t->second->kind == Real){
								SThead->data[pos].type = RealConst;
							} else if(t->second->kind == Record){
								SThead->data[pos].type = Record;
								tree temp = t->second;
								check(temp);
							} else if(t->second->kind == Boolean){
								SThead->data[pos].type = Boolean;
							} else {
								SThead->data[pos].type = NoType;
								fprintf(stderr, "Error: Type error assigning identifier %s\n",  id_name(t->first->value));
							}
						}
					}
				}
				break;
			}
			case Assert :
				if(check_expr(t->first) != Boolean){
					fprintf(stderr, "Error: Non-boolean in condition to ASSERT\n");
				}
				break;
			case Exit : {
				if(t->first != NULL){
					tree temp = t->first;
					if(t->first->kind == When){
						if(check_expr(t->first) != Boolean){
							fprintf(stderr, "Error: Non-boolean in condition to EXIT WHEN");
						}
					}
				}
				break;
			}
			case Begin : {
				currentScope++;
				int i, k;
				STentry newTable[100];
				push(&SThead, newTable);
				for(i = 0; i < 100; i++){
					SThead->data[i].index = 0;
					SThead->data[i].type = NoType;
					for(k = 0; k < 100; k++){
						SThead->data[i].fields[k].index = 0;
						SThead->data[i].fields[k].type = NoType;
					}
				}
				check(t->first);
				prST();
				pop(&SThead, SThead);	
				currentScope--;
				break;
			}
			case Bind :
				fprintf(stderr, "Bind is not yet implemented.\n");
	
				break;
			case Record : {
				tree temp = t->first;
				for(; temp != NULL; temp = temp->next){
					tree temp2 = temp->first;
					for(; temp2 != NULL; temp2 = temp2->next){
						int pos = temp2->value;
						if(SThead->data[recordDepth].fields[pos].index == 0){
							SThead->data[recordDepth].fields[pos].index = pos;
					
							if(temp->second->kind == Int){
								SThead->data[recordDepth].fields[pos].type = IntConst;
							} else if(temp->second->kind == Real){
								SThead->data[recordDepth].fields[pos].type = RealConst;
							} else if(temp->second->kind == Record){
								fprintf(stderr, "Error: Nested records not supported\n");
							} else if(temp->second->kind == Boolean){
								SThead->data[recordDepth].fields[pos].type = Boolean;
							} else {
								SThead->data[recordDepth].fields[pos].type = NoType;
								fprintf(stderr, "Error: Unrecognized type on record assignment\n");
							}
						} else {
							fprintf(stderr, "Error: record already declared in this scope\n");
						}
					}
				}
                                break;
			}
			case If : {
				if (check_expr (t->first) != Boolean){
					fprintf (stderr, "Error: Non-boolean in condition to IF\n");
				}
				check (t->second);
				if(t->third != NULL){
					check (t->third);
				}
				break;
			}
			case Then : case Else : {
				currentScope++;
				int i, k;
				STentry newTable[100];
				push(&SThead, newTable);
				for(i = 0; i < 100; i++){
					SThead->data[i].index = 0;
					SThead->data[i].type = NoType;
					for(k = 0; k < 100; k++){
						SThead->data[i].fields[k].index = 0;
						SThead->data[i].fields[k].type = NoType;
					}
				}
				check(t->first);
				prST();
				pop(&SThead, SThead);
				currentScope--;
				break;
			}
			case Elsif :
				if(check_expr(t->first) != Boolean)
					fprintf(stderr, "Error: Non-boolean in condition to ELSIF\n");
				check(t->second);
				check(t->third);
				break;
			case Loop : {
				currentScope++;
				int i, k;
				STentry newTable[100];
				push(&SThead, newTable);
				for(i = 0; i < 100; i++){
					SThead->data[i].index = 0;
					SThead->data[i].type = NoType;
					for(k = 0; k < 100; k++){
						SThead->data[i].fields[k].index = 0;
						SThead->data[i].fields[k].type = NoType;
					}
				}
				check(t->first);
				prST();
				pop(&SThead, SThead);
				currentScope--;
				break;
			}
			default :
				fprintf (stderr, "Error: invalid statement\n");
		}
	}
}
