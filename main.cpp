#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <unistd.h>

using namespace std;

int field[10][10] = {
    {NULL},
    {NULL,0,2,0,2,0,2,0,2},
    {NULL,2,0,2,0,2,0,2,0},
    {NULL,0,2,0,2,0,2,0,2},
    {NULL,0,0,0,0,0,0,0,0},
    {NULL,0,0,0,0,0,0,0,0},
    {NULL,1,0,1,0,1,0,1,0},
    {NULL,0,1,0,1,0,1,0,1},
	{NULL,1,0,1,0,1,0,1,0}};
	/*
	{
    {NULL},
    {NULL,0,0,0,0,0,0,0,0},
    {NULL,1,0,1,0,0,0,0,0},
    {NULL,1,0,0,2,0,4,0,0},
    {NULL,0,1,0,0,0,0,0,0},
    {NULL,1,0,0,0,0,0,0,2},
    {NULL,0,0,0,0,0,0,1,0},
    {NULL,0,0,0,1,0,1,0,0},
	{NULL,0,0,0,0,0,0,0,0}};*/

map<char, int> columnsToNumbers;
int checker_count_black = 12;// 1 = black, 3 = super black
int checker_count_white = 12;// 2 = white, 4 = super white
int currentPlayer = 1;
int stuckEnemy = 0;

void getInput();
void switchPlayer();
void movement(int y1, int x1, int y2, int x2);
int badMovementCheck(int y1, int x1, int y2, int x2);
int attackPossibility(int x, int y, bool moveStart);
void checkSetSuper();
int superMovement(int y1, int x1, int y2, int x2, bool simulate);
int getCurrentChecker();
int getCurrentSuperChecker();
int getCurrentEnemyChecker();
int getCurrentEnemySuperChecker();
void stuck();
int allMoves(int x, int y);
void endTurn();
void raiseError(int errorNumber);

int checkInput(int y, int x)
{
    if(x > 8 && x < 1)
    {
        raiseError(14);
        return 2;
    }
    else if(y > 8 && y < 1)
    {
        raiseError(14);
        return 2;
    }
    return 1;
}

void renderField()
{
    system("CLS");

    if(currentPlayer == 1)
    {
        cout<<"Current player Mr.White aka 'w'"<<endl<<endl;
    }
    else
    {
        cout<<"Current player Mr.Black aka 'a'"<<endl<<endl;
    }

    cout<<"Black checkers left: "<<checker_count_black<<endl;
    cout<<"White checkers left: "<<checker_count_white<<endl;

    int sizeOfArray = 40;
    char const horizontal[40] = "   +---+---+---+---+---+---+---+---+\n";
    char const vertical[40]   = "|   |   |   |   |   |   |   |   |\n";
    char const columns[40]    = "   a   b   c   d   e   f   g   h\n";
    char vertical_temp[sizeOfArray];
    char horizontal_temp[sizeOfArray];

        for(int i = 1; i <= 8; i++)
        {
            copy(horizontal, sizeOfArray+horizontal, horizontal_temp);
            copy(vertical, sizeOfArray+vertical, vertical_temp);

            for(int placement = 1; placement <= 8 ; placement++)
            {
                if(field[i][placement] == 1){
                    vertical_temp[(placement*4)-2] = 'a';
                }
                else if(field[i][placement] == 2){
                    vertical_temp[(placement*4)-2] = 'w';
                }
                else if(field[i][placement] == 3){
                    vertical_temp[(placement*4)-2] = '@';
                }
                else if(field[i][placement] == 4){
                    vertical_temp[(placement*4)-2] = 'M';
                }
            }

            cout<<horizontal_temp;
            cout<<" "<<i<<" ";
            cout<<vertical_temp;
        }
    cout<<horizontal_temp;
    cout<<"  "<<columns;

    /*for(int i = 1; i <= 8; i++)
        {cout<<endl;
            for(int j = 1;j <= 8; j++)
            {
                cout<<field[i][j];
            }
        }
    cout<<endl;*/
}

void gameEnd()
{
    checker_count_black = 0;
    checker_count_white = 0;

    stuck();
    for(int i = 1; i <= 8; i++)
    {
        for(int j = 1;j <= 8; j++)
        {
            if(field[i][j] == 1 || field[i][j] == 3)
            {
                checker_count_black++;
            }
            else if(field[i][j] == 2 || field[i][j] == 4)
            {
                checker_count_white++;
            }
        }
    }

    renderField();
    if(currentPlayer == 1 && checker_count_black - stuckEnemy == 0)
    {
        cout<<"WHITE WON!!!";
        getch();
        exit(0);
    }
    else if(currentPlayer == 2 && checker_count_white - stuckEnemy == 0)
    {
        cout<<"BLACK WON!!";
        getch();
        exit(0);
    }
}

int main()
{

    columnsToNumbers.insert(pair<char, int>('a', 1));
    columnsToNumbers.insert(pair<char, int>('b', 2));
    columnsToNumbers.insert(pair<char, int>('c', 3));
    columnsToNumbers.insert(pair<char, int>('d', 4));
    columnsToNumbers.insert(pair<char, int>('e', 5));
    columnsToNumbers.insert(pair<char, int>('f', 6));
    columnsToNumbers.insert(pair<char, int>('g', 7));
    columnsToNumbers.insert(pair<char, int>('h', 8));

    renderField();

while(1 < 10)
{
    if(attackPossibility(1, 1, true) == 0 )
    {
        getInput();
    }
    switchPlayer();
}
return 0;
}

void switchPlayer()
{
    if(currentPlayer==1)
    {
        currentPlayer=2;
    }
    else currentPlayer=1;
}

void movement(int y1, int x1, int y2, int x2)
{
    int moveDiffY = abs(y1-y2);
    int moveDiffX = abs(x1-x2);
    int killCount = 0;

    field[x2][y2] = field[x1][y1];
    field[x1][y1] = 0;
    if(moveDiffY == 2)
    {
        field[(x1 + x2)/2][(y1 + y2)/2] = 0;
        killCount++;
    }

    checkSetSuper();
    gameEnd();
    //endTurn();
    if(killCount == 1)
    {
        attackPossibility(x2, y2, false);
    }
}

int superMovement(int y1, int x1, int y2, int x2, bool simulate)
{
    int killCount = 0;
    if(simulate == false)
    {
        field[x2][y2] = field[x1][y1];
        field[x1][y1] = 0;
    }

    int Yaxis = y2;

        if(x1 > x2)
        {
            if(y1 > y2)
            {
                Yaxis++;
                for(int i = x2+1; i < x1; i++)
                {
                    //cout<<field[i][Yaxis]<<" "<<i<<" "<<Yaxis<<" 1op"<<endl;
                    //getch();
                    if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                    {
                        if(simulate == false)
                        {
                            field[i][Yaxis] = 0;
                            killCount++;
                        }
                        else return 1;
                    }
                    Yaxis++;
                }
            }
            else
            {
                Yaxis--;
                for(int i = x2+1; i < x1; i++)
                {
                    //cout<<field[i][Yaxis]<<" "<<i<<" "<<Yaxis<<" 2op"<<endl;
                    //getch();
                    if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                    {
                        if(simulate == false)
                        {
                            field[i][Yaxis] = 0;
                            killCount++;
                        }
                        else return 1;
                    }
                    Yaxis--;
                }
            }
        }
        else
        {

            if(y1 > y2)
            {
                Yaxis++;
                for(int i = x2-1; i > x1; i--)
                {
                    //cout<<field[i][Yaxis]<<" "<<i<<" "<<Yaxis<<" 3op"<<endl;
                    //getch();
                    if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                    {
                        if(simulate == false)
                        {
                            field[i][Yaxis] = 0;
                            killCount++;
                        }
                        else return 1;
                    }
                    Yaxis++;
                }
            }
            else
            {
                Yaxis--;
                for(int i = x2-1; i > x1; i--)
                {
                    //cout<<field[i][Yaxis]<<" "<<i<<" "<<Yaxis<<" 4op"<<endl;
                    //getch();
                    if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                    {
                        if(simulate == false)
                        {
                            field[i][Yaxis] = 0;
                            killCount++;
                        }
                        else return 1;
                    }
                    Yaxis--;
                }
            }
        }
    if(simulate == false)
    {
        gameEnd();
        if(killCount == 1)
        {
            attackPossibility(x2, y2, false);
        }
    }
}
int badMovementCheck(int y1, int x1, int y2, int x2)
{
    int moveDiffY = abs(y1-y2);
    int moveDiffX = abs(x1-x2);

    if(moveDiffY != moveDiffX)
    {
        return 5;
    }
    else if(field[x1][y1] == 0)
    {
        return 6;
    }
    else if(field[x1][y1] != getCurrentChecker() && field[x1][y1] != getCurrentSuperChecker())
    {
        return 7;
    }
    else if(field[x2][y2] == 1 || field[x2][y2] == 2 || field[x2][y2] == 3 || field[x2][y2] == 4)
    {
        return 8;
    }
    else if(field[x1][y1] == 1 || field[x1][y1] == 2)
    {
        if(currentPlayer == 1)
        {
            if(x1 > x2 && field[(x1 + x2)/2][(y1 + y2)/2] != 1 && field[(x1 + x2)/2][(y1 + y2)/2] != 3)
            {
                return 9;
            }
            else if(moveDiffY > 2)
            {
                return 10;
            }
            else if(moveDiffY == 2)
            {
                if(field[(x1 + x2)/2][(y1 + y2)/2] != 1 && field[(x1 + x2)/2][(y1 + y2)/2] != 3)
                {
                    return 11;
                }
            }
        }
        else
        {
            if(x1 < x2 && field[(x1 + x2)/2][(y1 + y2)/2] != 2 && field[(x1 + x2)/2][(y1 + y2)/2] != 4)
            {
                return 9;
            }
            else if(moveDiffY > 2)
            {
                return 10;
            }
            else if(moveDiffY == 2)
            {
                if(field[(x1 + x2)/2][(y1 + y2)/2] != 2 && field[(x1 + x2)/2][(y1 + y2)/2] != 4)
                {
                    return 11;
                }
            }
        }
    }
    else if(field[x1][y1] == 3 || field[x1][y1] == 4)
    {
        int Yaxis = y2;
        int enemyCheckersOnTheWay = 0;
        if(x1 > x2)
        {
            if(y1 > y2)
            {
                for(int i = x2; i < x1; i++)
                {
                        if(field[i][Yaxis] == getCurrentChecker() || field[i][Yaxis] == getCurrentSuperChecker())
                        {
                            return 12;
                        }
                        else if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                        {
                            enemyCheckersOnTheWay++;
                            if(enemyCheckersOnTheWay > 1)
                            {
                                return 13;
                            }
                        }
                    Yaxis++;
                }
            }
            else
            {
                for(int i = x2; i < x1; i++)
                {

                        if(field[i][Yaxis] == getCurrentChecker() || field[i][Yaxis] == getCurrentSuperChecker())
                        {
                            return 12;
                        }
                        else if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                        {
                            enemyCheckersOnTheWay++;
                            if(enemyCheckersOnTheWay > 1)
                            {
                                return 13;
                            }
                        }
                    Yaxis--;
                }
            }
        }
        else
        {
            if(y1 > y2)
            {
                for(int i = x2; i > x1; i--)
                {
                        if(field[i][Yaxis] == getCurrentChecker() || field[i][Yaxis] == getCurrentSuperChecker())
                        {
                            return 12;
                        }
                        else if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                        {
                            enemyCheckersOnTheWay++;
                            if(enemyCheckersOnTheWay > 1)
                            {
                                return 13;
                            }
                        }
                    Yaxis++;
                }
            }
            else
            {
                for(int i = x2; i > x1; i--)
                {
                        if(field[i][Yaxis] == getCurrentChecker() || field[i][Yaxis] == getCurrentSuperChecker())
                        {
                            return 12;
                        }
                        else if(field[i][Yaxis] == getCurrentEnemyChecker() || field[i][Yaxis] == getCurrentEnemySuperChecker())
                        {
                            enemyCheckersOnTheWay++;
                            if(enemyCheckersOnTheWay > 1)
                            {
                                return 13;
                            }
                        }
                    Yaxis--;
                }
            }
        }
    }
    return 1;
}

void getInput()
{
        renderField();

            int inputColumnFrom;
            int inputColumnTo;

            char inputChar;
            int inputIntFrom;
            int inputIntTo;

            int checkFinal = 0;
            int checkFrom;
            int checkTo;

            while( checkFinal != 1)
            {
                checkFrom = 0;
                checkTo = 0;
                while( checkFrom != 1)
                {
                    renderField();
                    cout<<"Which checker you want to move: ";
                    cin>>inputChar>>inputIntFrom;
                    getch();
                    auto x = columnsToNumbers.find(inputChar);
                    inputColumnFrom = x->second;
                    checkFrom = checkInput(inputColumnFrom,inputIntFrom);
                }

                while( checkTo != 1)
                {
                cout<<"Where do you want to move it: ";
                cin>>inputChar>>inputIntTo;
                getch();
                auto x = columnsToNumbers.find(inputChar);
                inputColumnTo = x->second;
                checkTo = checkInput(inputColumnTo,inputIntTo);
                }

                checkFinal = badMovementCheck(inputColumnFrom,inputIntFrom,inputColumnTo,inputIntTo);
                if(checkFinal != 1)
                {
                    raiseError(checkFinal);
                }
            }

            if(field[inputIntFrom][inputColumnFrom] == 3 || field[inputIntFrom][inputColumnFrom] == 4)
            {
                superMovement(inputColumnFrom, inputIntFrom ,inputColumnTo, inputIntTo, false);
            }
            else
            {
                movement(inputColumnFrom, inputIntFrom ,inputColumnTo, inputIntTo);
            }
}

int attackPossibility(int x, int y, bool moveStart)
{
    renderField();
    int attackNeeded = 0;
    int possibleAttacks = 0;
    int attackPresetX1, attackPresetX2, attackPresetY1, attackPresetY2 ;
    if(moveStart == true)
    {
        for(int i = 1; i <= 8; i++)
        {
            for(int j = 1; j <= 8; j++)
            {

                        if(field[i][j] == getCurrentChecker() || field[i][j] == getCurrentSuperChecker())
                        {
                            for(int b = 1; b <= 8; b++)
                            {
                                for(int c = 1; c <= 8; c++)
                                {
                                    if(1 == badMovementCheck(j, i, c, b))
                                    {
                                        if(field[i][j]==getCurrentChecker() && abs(i-b) > 1)
                                        {
                                            attackPresetX1 = i;
                                            attackPresetX2 = b;
                                            attackPresetY1 = j;
                                            attackPresetY2 = c;
                                            possibleAttacks++;
                                        }
                                        else if(field[i][j] == getCurrentSuperChecker() && abs(i-b) > 1)
                                        {
                                            if(superMovement(j, i, c, b, true) == 1)
                                            {
                                                attackPresetX1 = i;
                                                attackPresetX2 = b;
                                                attackPresetY1 = j;
                                                attackPresetY2 = c;
                                                possibleAttacks++;
                                            }
                                        }
                                    }
                                }
                            }
                        }

            }
        }

        if(possibleAttacks == 1)
        {
            if(field[attackPresetX1][attackPresetY1] == 3 || field[attackPresetX1][attackPresetY1] == 4)
            {
                cout<<"You have to attack!";
                getch();
                superMovement(attackPresetY1, attackPresetX1, attackPresetY2, attackPresetX2, false);
                attackNeeded = 1;
            }
            else
            {
                cout<<"You have to attack!";
                getch();
                movement(attackPresetY1, attackPresetX1, attackPresetY2, attackPresetX2);
                attackNeeded = 1;
            }
        }
        else if(possibleAttacks > 1)
        {
            cout<<"You have multiple attack options"<<endl;
            getch();
            attackNeeded = 0;
        }
    }
    else
    {
        //cout<<field[x][y]<<" "<<x<<" "<<y<<endl; getch();
        for(int i = 1 ; i <= 8 ; i++)
        {
            for(int j = 1; j <= 8; j++)
            {
                if(badMovementCheck(y, x, j, i) == 1)
                {
                    if(field[x][y]==getCurrentChecker() && abs(x-i) > 1)
                    {
                        attackPresetX1 = x;
                        attackPresetX2 = i;
                        attackPresetY1 = y;
                        attackPresetY2 = j;
                        possibleAttacks++;
                    }
                    else if(field[x][y] == getCurrentSuperChecker() && abs(x-i) > 1)
                    {
                        if(superMovement(y, x, j, i, true) == 1)
                        {
                            attackPresetX1 = x;
                            attackPresetX2 = i;
                            attackPresetY1 = y;
                            attackPresetY2 = j;
                            possibleAttacks++;

                            //cout<<possibleAttacks<<endl;
                            //cout<<attackPresetX1<<" "<<attackPresetY1<<endl;
                            //cout<<attackPresetX2<<" "<<attackPresetY2<<endl;getch();
                        }
                    }
                }
            }
        }
        if(possibleAttacks == 1)
        {
            if(field[attackPresetX1][attackPresetY1] == 3 || field[attackPresetX1][attackPresetY1] == 4)
            {
                cout<<"You have to attack!";
                getch();
                superMovement(attackPresetY1, attackPresetX1, attackPresetY2, attackPresetX2, false);
            }
            else
            {
                cout<<"You have to attack!";
                getch();
                movement(attackPresetY1, attackPresetX1, attackPresetY2, attackPresetX2);
            }
        }
        else if(possibleAttacks > 1)
        {
            cout<<"You have multiple attack options!"<<endl;
            getch();
            getInput();
        }
    }
return attackNeeded;
}

int getCurrentChecker()
{
    if(currentPlayer == 1)
    {
        return 2;
    }
    else return 1;
}

int getCurrentSuperChecker()
{
    if(currentPlayer == 1)
    {
        return 4;
    }
    else return 3;
}

int getCurrentEnemyChecker()
{
    if(currentPlayer == 1)
    {
        return 1;
    }
    else return 2;
}

int getCurrentEnemySuperChecker()
{
    if(currentPlayer == 1)
    {
        return 3;
    }
    else return 4;
}

void stuck()
{
    stuckEnemy = 0;
    for(int i = 1; i <= 8; i++)
    {
        for(int j = 1; j <= 8; j++)
        {
            if(field[i][j] == getCurrentEnemyChecker() || field[i][j] == getCurrentEnemySuperChecker())
            {
                if(allMoves(i,j) == 2)
                {
                    stuckEnemy++;
                }
            }
        }
    }
}

int allMoves(int x, int y)
{
    switchPlayer();
    for(int i = 1; i <= 8; i++)
    {
        for(int j = 1; j <= 8; j++)
        {
            if(badMovementCheck(y, x, j, i) == 1)
            {
                switchPlayer();
                return 1;
            }
        }
    }
    switchPlayer();
return 2;
}

void endTurn()
{
    renderField();
    cout<<"Do you want to end your turn? (Y/N)"<<endl;
    char turn;
    turn = getch();
    if(turn == 'N' || turn == 'n')
    {
        getInput();
    }
}

void checkSetSuper()
{
        for(int j = 1;j <= 8; j++)
        {
            if(field[1][j] == 1)
            {
                field[1][j] = 3;
            }
            else if(field[8][j] == 2)
            {
                field[8][j] = 4;
            }
        }
    renderField();
}

void raiseError(int errorNumber)
{
    switch(errorNumber){
    case 5:
        cout<<"Error! You can't move like that";
        break;
    case 6:
        cout<<"Bad input!! You want to move void :)";
        break;
    case 7:
        cout<<"IT'S NOT YOUR CHECKER, BOI";
        break;
    case 8:
        cout<<"Error! You can't move to cell with checker ";
        break;
    case 9:
        cout<<"Error! You can't move backwards or stay";
        break;
    case 10:
        cout<<"Error! You can't move that far ";
        break;
    case 11:
        cout<<"Error! You can't move there ";
        break;
    case 12:
        cout<<"There is a checker on your way !";
        break;
    case 13:
        cout<<"There is too much enemies on your way !";
        break;
    case 14:
        cout<<"Bad input!! Your move is not on the field. Press any key to try again!";
        break;
    }
    getch();
}
