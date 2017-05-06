#!/usr/bin/env python
from __future__ import print_function, unicode_literals
from sys import argv
import z3


def parse_spec(n, m):
    sdk_spec = {}
    for row in range(n * m):
        line = raw_input()
        elems = line.split(' ')
        if len(elems) != n * m:
            print('Error: wrong number of inputs.' +
                  ' Was expecting %d, but got %d.' % (n * m, len(elems)))
            exit(1)
        else:
            sdk_spec[row + 1] = dict(zip(range(1, n * m + 1), map(int, elems)))

    return sdk_spec


def solve(spec, n, m):
    solver = z3.Solver()

    # The function that will represent the solution.
    s = z3.Function('s', z3.IntSort(), z3.IntSort(), z3.IntSort())

    # Each cell must contain a number between 1 and n * m.
    for row in range(n * m):
        for col in range(n * m):
            solver.add(z3.And(s(row + 1, col + 1) >= 1,
                              s(row + 1, col + 1) <= n * m))

    # Restrictions of distinctness in rows.
    for row in range(n * m):
        row_vars = [s(row + 1, col + 1) for col in range(n * m)]
        solver.add(z3.Distinct(*row_vars))

    # Restrictions of distinctness in columns.
    for col in range(n * m):
        col_vars = [s(row + 1, col + 1) for row in range(n * m)]
        solver.add(z3.Distinct(*col_vars))

    # Restrictions of distinctness in subrectangles.
    for rect_row in range(m):
        for rect_col in range(n):
            cells = [s(cell_row + 1, cell_col + 1)
                     for cell_row in range(rect_row * n, rect_row * n + n)
                     for cell_col in range(rect_col * m, rect_col * m + m)]
            solver.add(z3.Distinct(*cells))

    # Restrictions from the user.
    for row in range(1, n * m + 1):
        for col in range(1, n * m + 1):
            if spec[row][col]:
                solver.add(s(row, col) == spec[row][col])

    is_sat = solver.check()
    if is_sat == z3.sat:
        m = solver.model()
        return m[s]
    else:
        return None


def pretty_print(s):
    if s is None:
        print('Puzzle is unsolvable.')
        return

    entries_list = []
    for i in range(s.num_entries()):
        e = map(int, map(z3.IntNumRef.as_long, s.entry(i).as_list()))
        entries_list.append(((e[0], e[1]), e[2]))
    entries = dict(entries_list)

    for row in range(n * m):
        if row % n == 0:
            print('--' * (n * m + n) + '-')
        print(': ', end='')
        for col in range(n * m):
            print('%d' % entries[(row + 1, col + 1)], end='')
            if (col + 1) % m != 0:
                print(' ', end='')
            else:
                print(' : ', end='')
        print('')
    print('--' * (n * m + n) + '-')


if __name__ == '__main__':
    if len(argv) != 3:
        print("usage: %s n m" % argv[0])
        exit(1)

    n = int(argv[1])
    m = int(argv[2])

    spec = parse_spec(n, m)
    s = solve(spec, n, m)
    pretty_print(s)
