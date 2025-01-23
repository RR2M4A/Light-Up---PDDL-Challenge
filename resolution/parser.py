import sys
import os

DOMAIN_STR = """(define
    (domain domain-lightsup)
    (:requirements :strips :adl :typing :negative-preconditions)
    (:types
        position counter
    )

    (:constants i0 i1 i2 i3 i4 - counter)
    
    (:predicates
        (cell-lightened ?p - position)
        (counter-at ?p - position ?c - counter)
    )
actions_here
)"""

PROBLEM_STR = """(define
        (problem problem-lightsup)
        (:domain domain-lightsup)
        (:objects )
        (:init )
        (:goal (and )))"""


def get_input():

    vet = []

    for index, line in enumerate(sys.stdin):

        vet.append([])

        for char in line:
            if char != '\n':
                vet[index].append(char)

    return vet


def get_name(symbol: str, line: str, col: str) -> str:
    if symbol.isdigit():
        name = f"p_{line}_{col}_{symbol}"

    elif symbol == "#" or symbol == '-':
        name = f"p_{line}_{col}"

    else:
        name = None

    return name


def add_in_dict(my_dict: dict, position: str, block: str = "", numbered: str = ""):

    if position not in my_dict:
        my_dict[position] = [set(), set()]
        
    if block:
        my_dict[position][0].add(block)
    if numbered:
        my_dict[position][1].add(numbered)


def add_cell_visibility(main_dict: dict, board: list):

    # Loop em linhas
    for i in range(len(board)):
        for j in range(len(board[0])):

            current_cell = board[i][j]

            if current_cell == '-':
                current_cell_name = get_name(current_cell, i, j)
                add_in_dict(main_dict, current_cell_name)

                # Loop para esquerda
                l = j - 1
                while l >= 0:
                    previous_cell = board[i][l]

                    if previous_cell == '-':
                        previous_cell_name = get_name(previous_cell, i, l)
                        add_in_dict(main_dict, current_cell_name, previous_cell_name)
                    else:
                        break

                    l -= 1

                # Loop para direita
                r = j + 1
                while r < len(board[0]):
                    next_cell = board[i][r]

                    if next_cell == '-':
                        next_cell_name = get_name(next_cell, i, r)
                        add_in_dict(main_dict, current_cell_name, next_cell_name)
                    else:
                        break

                    r += 1

                # Loop para cima
                u = i - 1
                while u >= 0:
                    previous_cell = board[u][j]

                    if previous_cell == '-':
                        previous_cell_name = get_name(previous_cell, u, j)
                        add_in_dict(main_dict, current_cell_name, previous_cell_name)
                    else:
                        break

                    u -= 1

                # Loop para baixo
                d = i + 1
                while d < len(board):
                    next_cell = board[d][j]

                    if next_cell == '-':
                        next_cell_name = get_name(next_cell, d, j)
                        add_in_dict(main_dict, current_cell_name, next_cell_name)
                    else:
                        break

                    d += 1
    

def add_numered(main_dict: dict, board: list):

    for i in range(len(board)):
        for j in range(len(board[0])):

            current_cell = board[i][j]

            if current_cell == '-':

                # Verificando se há células numeradas (cima, baixo, direita, esquerda)
                # Cima
                if i - 1 > -1:
                    if board[i - 1][j].isdigit():
                        numbered_cell_name = get_name(board[i - 1][j], i - 1, j)
                        current_cell_name = get_name(current_cell, i, j)

                        add_in_dict(main_dict, current_cell_name, numbered=numbered_cell_name)

                # Baixo
                if i + 1 < len(board):
                    if board[i + 1][j].isdigit():
                        numbered_cell_name = get_name(board[i + 1][j], i + 1, j)
                        current_cell_name = get_name(current_cell, i, j)

                        add_in_dict(main_dict, current_cell_name, numbered=numbered_cell_name)

                # Esquerda
                if j - 1 > -1:
                    if board[i][j - 1].isdigit():
                        numbered_cell_name = get_name(board[i][j - 1], i, j - 1)
                        current_cell_name = get_name(current_cell, i, j)

                        add_in_dict(main_dict, current_cell_name, numbered=numbered_cell_name)

                # Direita
                if j + 1 < len(board[0]):
                    if board[i][j + 1].isdigit():
                        numbered_cell_name = get_name(board[i][j + 1], i, j + 1)
                        current_cell_name = get_name(current_cell, i, j)

                        add_in_dict(main_dict, current_cell_name, numbered=numbered_cell_name)


def store_positions_info(main_dict: dict, board: list):

    add_cell_visibility(main_dict, board)
    add_numered(main_dict, board)


def update_domain_str(main_dict: dict):

    actions = ""

    for key, value in main_dict.items():

        preconditions = f"(not (cell-lightened {key}))"
        
        effect = " ".join(f"(cell-lightened {cell})" for cell in value[0])
        effect +=  f" (cell-lightened {key})"
        effect_part_two = ""

        for numbered_cell in value[1]:
            effect_part_two += f"""(when (counter-at {numbered_cell} i0) (and (not (counter-at {numbered_cell} i0)) (counter-at {numbered_cell} i1)))
            (when (counter-at {numbered_cell} i1) (and (not (counter-at {numbered_cell} i1)) (counter-at {numbered_cell} i2)))
            (when (counter-at {numbered_cell} i2) (and (not (counter-at {numbered_cell} i2)) (counter-at {numbered_cell} i3)))
            (when (counter-at {numbered_cell} i3) (and (not (counter-at {numbered_cell} i3)) (counter-at {numbered_cell} i4)))\n\t\t"""

        actions += f"""(:action {key}

        :parameters()
        :precondition (and {preconditions})
        :effect(and {effect + effect_part_two})
)\n\n"""

    new_domain_str =  DOMAIN_STR.replace("actions_here", actions)
    return new_domain_str


def create_domain_file(main_dict: dict):

    domain_str = update_domain_str(main_dict)

    with open("domain.pddl", "w") as domain_file:
        domain_file.write(domain_str)


def update_problem_str(main_dict: dict):
    positions = ''
    init = ''
    goal = ''

    for key, value in main_dict.items():
        
        positions += key + " "

        goal += f"(cell-lightened {key})\n"

        for numbered_cell in value[1]:
            if numbered_cell not in init:
                init += f"(counter-at {numbered_cell} i0)\n"
                positions += numbered_cell + " "
            
            if numbered_cell not in goal:
                goal += f"(counter-at {numbered_cell} i{numbered_cell[-1]})\n"

    positions += '- position'

    new_problem_str = PROBLEM_STR.replace("objects ", f"objects {positions}")
    new_problem_str = new_problem_str.replace("init ", f"init  {init}")
    new_problem_str = new_problem_str.replace("and ", f"and {goal}")

    return new_problem_str


def create_problem_file(main_dict: dict):
    
    problem_str = update_problem_str(main_dict)

    with open("problem.pddl", "w") as domain_file:
        domain_file.write(problem_str)


def cat_plan():
    
    if not os.path.exists("plan.txt"):
        exit(120)
    
    os.system("cat plan.txt | grep 'STEP' | tr -d '[:space:]' | sed -E 's/STEP[0-9]+://g; s/_/ /g; s/p/\\(bulb/g; s/\\(\\)\\(/\\);\\(/g; s/\\(\\)/\\)/g'")

if __name__ == "__main__":

    main_dict = {}
    board = get_input()

    # /tmp/dir/software/planners/downward/fast-downward.py --plan-file plan.txt --alias seq-opt-fdss-2023 --overall-time-limit 179s domain.pddl problem.pddl > /dev/null"
    # planners/madagascar/Mp -o plan.txt -S 1 domain.pddl problem.pddl"
    execution_str = "../planners/madagascar/Mp -o plan.txt -S 1 domain.pddl problem.pddl > /dev/null"

    store_positions_info(main_dict, board)
    create_domain_file(main_dict)
    create_problem_file(main_dict)
    os.system(execution_str)
    cat_plan()