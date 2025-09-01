# pip install sympy
# https://docs.sympy.org/latest/modules/logic.html

from sympy.logic.boolalg import to_anf, to_cnf, to_dnf, to_nnf
from sympy.logic.boolalg import simplify_logic, truth_table

# from sympy.abc import a1, b1, c1, d1 # dont work
from sympy.abc import a, b, c, d, t
from sympy.abc import A, B, C, D, T

# print(to_dnf(B & (A | C)))
# print(to_dnf(B & (A | C), simplify=True))
# print(to_dnf(A | (A & B)))
# print(to_dnf(A | (A & B), simplify=True))

t = (a >> C) & ((~a) >> B)
T = (A >> c) & ((~A) >> b)

# table = truth_table(a >> (D | t), [a, D, t])
# for row in table:
# 	print("{0} -> {1}".format(*row))

print(to_dnf(
	a >> (D | t)
	, simplify=True)) # yes!

print(to_dnf(
	A >> (d | T)
	, simplify=True)) # yes!

print(to_dnf(
	b | B | t | T
	, simplify=True)) # cool

# Total expression:
total = (a >> (D | t)) & (A >> (d | T)) & (b | B | t | T) # wrong!

# print("\nto_anf:\n", to_anf(total)) # no simplify

# print("\nto_cnf:\n", to_cnf(total, simplify=True))

# print("\nto_dnf:\n", to_dnf(total, simplify=True))

# print("\nto_nnf:\n", to_nnf(total, simplify=True))

print("\nsimplify_logic:\n", simplify_logic(total))


total = (a >> (D | t)) & ((~a) >> (A >> (d | T))) & (((~a) & (~A)) >> (b | B | t | T))
print("\nsimplify_logic:\n", simplify_logic(total))

from sympy.logic.boolalg import ITE
# If-then-else clause.

total = ITE(a, D | t, ITE(A, d | T, b | B | t | T)) # correct
print("\nsimplify_logic:\n", simplify_logic(total)) # wow
