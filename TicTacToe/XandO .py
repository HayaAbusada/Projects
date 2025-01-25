def setup():
    counter = 0
    Board = [[0 for k in range(3)] for m in range(3)]
    for rows in range (0, 3):
        for columns  in range (0, 3):
            counter = counter + 1
            Board[rows][columns] = str(counter)
    Winner = False
    X_Count = 0
    O_Count = 0
    return Board, Winner, X_Count, O_Count

def character():
    Player = input("who is playing X or O?")
    while Player != "X" and Player != "O":
        Player = input("please enter 'X' or 'O'")
    if Player == "X":
        comp = "O"
        temp = "X"
    else:
        comp = "X"
        temp = "O"
    return Player, comp, temp

def menu():
    print("How many players:")
    print("1.) 1 player")
    print("2.) 2 players")
    number = input("enter number:")
    while number != "1" and number != "2":
        number = input("invalid, enter number:")
    number = int(number)
    print("would you like to play:")
    print("1.) Best out of 1")
    print("2.) Best out of 3")
    print("3.) Best out of 5")
    Best = input("enter option")
    while Best.isdigit() == False or int(Best) < 1 or int(Best) > 3:
        Best = input("invalid, please enter choice")
    if Best == "1":
        choice = 1
    elif Best == "2":
        choice = 3
    else:
        choice = 5
    return number, choice

def move(Board, Player, X_Count, O_Count):
    validation = False
    Found = False
    print ("its player", Player, "turn")
    position = input("what position would you like to move into (1 to 9)?")
    while validation == False:
        count = 0
        validation = True
        while position.isdigit() == False:
            position = input("invalid, please enter an integer from 1 to 9?")
        while Found == False and count < 9:
            for i in range (3):
                for j in range (3):
                    if Board[i][j] == position:
                        Found = True
                        Board[i][j] = Player
                        row = i
                        column = j
                    count = count + 1
        if Found == False and count == 9:
            position = input("position is taken or doesnt exist, please re-enter")
            validation = False
    check = Player 
    if Player == "X":
        X_Count = X_Count + 1
        Player = "O"
    else:
        O_Count = O_Count + 1
        Player = "X"
    return Board, row, column, X_Count, O_Count, check, Player

    
def single(Board, Player, X_Count, O_Count, comp, temp):
    validation = False
    two = False
    fill_c = 0
    fill_r = 0
    Found = False
    print ("its player", Player, "turn")
    if comp != Player:
        position = input("what position would you like to move into (1 to 9)?")
    else:
        k = 0
        while k < 3 and two == False:
            amount = 0
            for m in range(3):
                if Board[k][m] == temp:
                    amount = amount + 1
                else:
                    fill_r = k
                    fill_c = m
            if amount == 2 and Board[fill_r][fill_c] != comp:
                two = True
                Board[fill_r][fill_c] = comp
                row = fill_r
                column = fill_c
                k = 3
            k = k + 1
        p = 0
        while p < 3 and two == False:
            amount = 0
            for t in range(3):
                if Board[t][p] == temp:
                    amount = amount + 1
                else:
                    fill_r = t
                    fill_c = p
            if amount == 2 and Board[fill_r][fill_c] != comp:
                two = True
                Board[fill_r][fill_c] = comp
                row = fill_r
                column = fill_c
                p = 3
            p = p + 1
        l = 0
        v = 0
        amount = 0
        while l < 3 and two == False:
            if Board[l][v] == temp:
                amount = amount + 1
            else:
                fill_r = l
                fill_c = v
            if amount == 2 and Board[fill_r][fill_c] != comp:
                two = True
                Board[fill_r][fill_c] = comp
                row = fill_r
                column = fill_c
                l = 3
            l = l + 1
            v = v + 1
        y = 0
        c = 2
        amount = 0
        while y < 3 and two == False:
            if Board[c][y] == temp:
                amount = amount + 1
            else:
                fill_r = c
                fill_c = y
            if amount == 2 and Board[fill_r][fill_c] != comp:
                two = True
                Board[fill_r][fill_c] = comp
                row = fill_r
                column = fill_c
                y = 3
            y = y + 1
            c = c - 1
        if two == False:
            import random
            random = random.randint(1,9)
            position = str(random)         
    while validation == False:
        count = 0
        validation = True
        if two == False:
            while position.isdigit() == False:
                position = input("invalid, please enter an integer from 1 to 9?")
            while Found == False and count < 9:
                for i in range (3):
                    for j in range (3):
                        if Board[i][j] == position:
                            Found = True
                            Board[i][j] = Player
                            row = i
                            column = j
                        count = count + 1
            if Found == False and count == 9 and comp != Player:
                position = input("position is taken or doesnt exist, please re-enter")
                validation = False
            if Found == False and count == 9 and comp == Player:
                import random
                random = random.randint(1,9)
                position = str(random)
                validation = False
    check = Player 
    if Player == "X":
        X_Count = X_Count + 1
        Player = "O"
    else:
        O_Count = O_Count + 1
        Player = "X"
    return Board, row, column, X_Count, O_Count, check, Player

def win(Board, check, row, column, X_Count, O_Count, Winner):
    Draw = False
    if check == "X":
        count = X_Count
    else:
        count = O_Count
    if count >= 3:
        if Board[0][column] == check and Board[1][column] == check and Board[2][column] == check:
            Winner = True
            print("PLAYER", check, "WON!")
        elif Board[row][0] == check and Board[row][1] == check and Board[row][2] == check:
            Winner = True
            print("PLAYER", check, "WON!")
        elif Board[0][0] == check and Board[1][1] == check and Board[2][2] == check:
            Winner = True
            print("PLAYER", check, "WON!")
        elif Board[0][2] == check and Board[1][1] == check and Board[2][0] == check:
            Winner = True
            print("PLAYER", check, "WON!")
        elif (X_Count + O_Count) == 9:
            Winner = True
            Draw = True
            print("ITS A DRAW!")
        else:
            Winner = False
    return Winner, Draw


def output(Board):        
    print(Board[0][0] + " | " + Board[0][1] + " | " + Board[0][2])
    print("──────────")
    print(Board[1][0] + " | " + Board[1][1] + " | " + Board[1][2])
    print("──────────")
    print(Board[2][0] + " | " + Board[2][1] + " | " + Board[2][2])

while True:
    print("NEW GAME")
    X_win = 0
    O_win = 0
    Counter = 0
    game = 1
    number, choice = menu()
    if number == 1:
        while Counter < choice:
            print ("GAME", game)
            if Counter < 1:
                Player, comp, temp = character()
            Board, Winner, X_Count, O_Count = setup()
            print()
            output(Board)
            print()
            while Winner == False:
                Board, row, column, X_Count, O_Count, check, Player = single(Board, Player, X_Count, O_Count, comp, temp)
                print()
                Winner, Draw = win(Board, check, row, column, X_Count, O_Count, Winner)
                print()
                output(Board)
                print()
            if check == "X" and Draw == False and choice > 1:
                X_win = X_win + 1
            elif check == "O" and Draw == False and choice > 1:
                O_win = O_win + 1
            if choice == 3 and X_win == 2 or O_win == 3:
                Counter = 3
            elif choice == 5 and X_win == 3 or O_win == 3:
                Counter = 5
            Counter = Counter + 1
            game = game + 1
        if choice > 1:
            if X_win > O_win:
                print("PLAYER X WON OVERALL")
            elif O_win > X_win:
                print ("PLAYER O WON OVERALL")
            else:
                print("IT WAS A DRAW OVERALL")
        print() 
    else:
        while Counter < choice:
            print("GAME", game)
            Player, comp, temp = character()
            Board, Winner, X_Count, O_Count = setup()
            print()
            output(Board)
            print()
            while Winner == False:
                Board, row, column, X_Count, O_Count, check, Player = move(Board, Player, X_Count, O_Count)
                print()
                Winner, Draw = win(Board, check, row, column, X_Count, O_Count, Winner)
                print()
                output(Board)
                print()
            if check == "X" and Draw == False and choice > 1:
                X_win = X_win + 1
            elif check == "O" and Draw == False and choice > 1:
                O_win = O_win + 1
            if choice == 3 and X_win == 2 or O_win == 2:
                Counter = 3
            elif choice == 5 and X_win == 3 or O_win == 3:
                Counter = 5
            Counter = Counter + 1
            game = game + 1
        if choice > 1:
            if X_win > O_win:
                print("PLAYER X WON OVERALL")
            elif O_win > X_win:
                print ("PLAYER O WON OVERALL")
            else:
                print("IT WAS A DRAW OVERALL")
        print()
        
