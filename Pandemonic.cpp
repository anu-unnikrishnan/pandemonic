#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
using namespace std;

//size of grid
const int n = 8;

//prints character grid
void print(char grid[n][n])
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(grid[i][j] == 'p')
                cout<<"\u15DD"<<"  ";
            else if(grid[i][j] == 'o')
                cout<<"\u26C4"<<" ";
            else if(grid[i][j] == '|')
                cout<<"\u26F1"<<"  ";
            else if(grid[i][j] == 'h')
                cout<<"\u2764"<<"  ";
            else
               cout<<grid[i][j]<<"  ";
        }
        cout<<endl;
    }
}

//prints the grid at the stage of game over - with black circle for infected snowman
void printover(char grid[n][n], int perspos[2])
{
    cout<<endl;
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            if(i == perspos[0] && j == perspos[1])
                cout<<"\u25CF"<<"  ";
            else if(grid[i][j] == 'p')
                cout<<"\u15DD"<<"  ";
            else if(grid[i][j] == '|')
                cout<<"\u26F1"<<"  ";
            else if(grid[i][j] == 'h')
                cout<<"\u2764"<<"  ";
            else
                cout<<grid[i][j]<<"  ";
        }
        cout<<endl;
    }
    cout<<endl;
}

//prints integer grid - we use this to print the infections array
void printint(int grid[n][n])
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            cout<<grid[i][j]<<"  ";
        }
        cout<<endl;
    }
}

//counting number of 1s in collhealth matrix = amount of health
int countones(int collhealth[n][n])
{
    int i, j, noones = 0;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            if(collhealth[i][j] == 1)
                noones++;
        }
    }
    return noones;
}

//finds the position of the person in the grid
void findperson(char grid[n][n], int perspos[2])
{
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            if(grid[i][j] == 'o')
            {
                perspos[0] = i;
                perspos[1] = j;
            }
        }
    }
}

//infect cells of grid
void infect(char grid[n][n], char newgrid[n][n], int R0, int infections[n][n], int perspos[2])
{
    int i, j, k, l; //loop variables
    int c, d; //variables used to pick random neighbours
    
    //these arrays keep track of whether we randomly pick the same neighbour twice to get infected
    int carr[R0], darr[R0];
    int cctr, dctr;
    
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            //portal stays in the same position
            if(grid[i][j] == 'p')
                newgrid[i][j] = 'p';

            //for a particular infected cell
            //the infected cell infects R0 neighbours
            //if the cell is infected AND it hasn't already infected R0 neighbours
            if(grid[i][j] == 'X' && infections[i][j] != R0)
            {
                //that cell remains infected
                newgrid[i][j] = 'X';
                
                cctr = 0;
                dctr = 0;
                int flag;
                
                //initialise these with a value that will never be present normally eg. -5
                //these arrays are specific to each infected cell
                //as we want to keep track of how many other cells this cell will infect
                //and we are using these arrays to make sure the same neighbouring cell isn't randomly chosen more than once
                for(l = 0; l < R0; l++)
                {
                    carr[l] = -5;
                    darr[l] = -5;
                }
                
                //cout<<"The infected cell ["<<i<<"]["<<j<<"] infects..."<<endl;
                //keep track of how many neighbours that cell has infected
                //allowing the cell to infect at most R0 neighbours
                while(infections[i][j] < R0)
                {
                    //pick random number between -1 and 1
                    do
                    {
                        c = rand() % 3; //random number between 0 and 2
                        d = rand() % 3;
                        c--; //random number between -1 and 1
                        d--;
                        
                        //check if these values have already been used i.e. are they in carr, darr
                        flag = 0;
                        for(k = 0; k < R0; k++)
                        {
                            //if these values c and d have already been chosen for that particular infected cell
                            if(carr[k] == c && darr[k] == d)
                                flag = 1;
                        }
                    //so that we don't choose a health supply cell
                    //and so that we don't choose a social distancing cell
                    //and so that the portal doesn't get infected
                    //and so that we don't choose a previously chosen neighbour
                    //and so that we don't choose the cell itself
                    //and so that our chosen neighbouring cell lies in the grid
                    }while((grid[i+c][j+d] == 'h') || (grid[i+c][j+d] == '|') || (grid[i+c][j+d] == 'p') || (flag == 1) || ((c == 0) && (d == 0)) || (i+c >= n) || (i+c < 0) || (j+d >= n) || (j+d < 0));
                    
                    //infect that randomly chosen neighbouring cell
                    //cout<<"infecting cell ["<<i+c<<"]["<<j+d<<"]!"<<endl;
                    newgrid[i+c][j+d] = 'X';

                    //store these values in arrays so that we can check if they've previously been chosen
                    carr[cctr] = c;
                    darr[dctr] = d;
                    cctr++;
                    dctr++;
                    
                    //-1 means that cell is not infected
                    //0 means that cell is infected but hasn't infected anything else yet
                    //need this distinction so that we can check if the virus has fully spread later
                    if(infections[i+c][j+d] == -1)
                        infections[i+c][j+d] = 0;
                    
                    //increment number of infected neighbours
                    infections[i][j]++;
                }
            }
        }
    }
}

//outputs 0 if full spread has occurred
int spread(int infgrid[n][n], int R0)
{
    int i, j, c, d;
    int spreadctr = 0;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            //for any remaining uninfected cell
            if(infgrid[i][j] == -1)
            {
                //check if all of its neighbours have infected R0 cells already
                for(c = -1; c <= 1; c++)
                {
                    for(d = -1; d <= 1; d++)
                    {
                        //if any neighbour hasn't yet infected R0 cells
                        //and if we're not talking about the cell itself
                        if(infgrid[i+c][j+d] != R0 && ((c != 0) || (d != 0)))
                        {
                            //if neighbour is a valid cell
                            if((i+c < n) && (j+d < n) && (i+c >= 0) && (j+d >= 0))
                            {
                                //if neighbour is not an uninfected cell
                                if(infgrid[i+c][j+d] != -1)
                                    spreadctr++;
                            }
                        }
                    }
                }
            }
        }
    }
    //if spreadctr is 0, this means the infection has fully spread
    if(spreadctr == 0)
        return 0;
    //if spreadctr isn't 0, this means it can still keep going
    else
        return 1;
}

//counts number of infected cells
int count(char grid[n][n])
{
    int infctr = 0;
    int i, j;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            if(grid[i][j] == 'X')
                infctr++;
        }
    }
    return infctr;
}

//checks if person is infected
int infectedperson(char grid[n][n], int perspos[2])
{
    findperson(grid, perspos);
    int i, j, k, l;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            //this means the person is infected
            if(grid[i][j] == 'X' && perspos[0] == i && perspos[1] == j)
                return 1;
        }
    }
        //this means the person is not infected
        return 0;
}

//check if valid move
int checkvalidmove(int x)
{
    if(x < 0 || x >= n)
    {
        cout<<endl<<"That's an invalid move, try again."<<endl;
        return 1;
    }
    return 0;
}

void gameover(char grid[n][n], int perspos[2], int step)
{
    printover(grid, perspos);
    cout<<"Uh-oh! The \u26C4 is infected after "<<step<<" hour";
    if(step != 1)
    cout<<"s";
    cout<<"."<<endl<<"GAME OVER."<<endl<<endl;
    exit(0);
}

//use the first health supply we find - set the value to -1
void usehealthsupply(int collhealth[n][n])
{
    int i, j, counter;
    counter = 0;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            if(collhealth[i][j] == 1)
            {
                collhealth[i][j] = -1;
                counter = 1;
                break;
            }
        }
        if(counter == 1)
            break;
    }
}

//move snowman through the grid according to user input
void moveperson(char grid[n][n], int perspos[2], int step, int collhealth[n][n], int &cured)
{
    char choice;
    int i, j, valid, counter;
    do
    {
        valid = 0;
        cout<<endl<<"Where do you want to move?"<<endl;
        cout<<"l: left, r: right, u: up, d: down, s: stay, g: give up"<<endl;
        cin>>choice;

        findperson(grid, perspos);
        
        switch(choice)
        {
            case 'l':
                //check if valid move
                if(checkvalidmove(perspos[1]-1) == 1)
                    break;
                //if moved into an infected cell and no collected health supplies left
                else if(grid[perspos[0]][perspos[1]-1] == 'X' && countones(collhealth) == 0)
                {
                    grid[perspos[0]][perspos[1]] = '.';
                    perspos[1] = perspos[1]-1;
                    gameover(grid, perspos, step);
                }
                //if moved into an infected cell and collected health supplies left
                else if(grid[perspos[0]][perspos[1]-1] == 'X' && countones(collhealth) != 0)
                {
                    usehealthsupply(collhealth);
                    //move to that cell
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]][perspos[1]-1] = 'o';
                    valid = 1;
                    cured = 1;
                    findperson(grid, perspos);
                    printover(grid, perspos);
                    cout<<"The \u26C4 is infected, but it can use a \u2764 to recover."<<endl;
                }
                //if moved into a social distancing cell
                else if(grid[perspos[0]][perspos[1]-1] == '|')
                {
                    cout<<endl<<"The \u26C4 can't go there, as that person is in self-isolation."<<endl;
                }
                //if moved into a health supply cell
                else if(grid[perspos[0]][perspos[1]-1] == 'h')
                {
                    //increment number of collected health supplies
                    collhealth[perspos[0]][perspos[1]-1] = 1;
                    //move to new cell
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]][perspos[1]-1] = 'o';
                    cout<<endl<<"You've collected a \u2764!"<<endl;
                    valid = 1;
                }
                //if valid and not infected
                else
                {
                    //move the person from the old cell
                    grid[perspos[0]][perspos[1]] = '.';
                    //to the new cell
                    grid[perspos[0]][perspos[1]-1] = 'o';
                    valid = 1;
                }
                break;
            case 'r':
                if(checkvalidmove(perspos[1]+1) == 1)
                    break;
                else if(grid[perspos[0]][perspos[1]+1] == 'X' && countones(collhealth) == 0)
                {
                    grid[perspos[0]][perspos[1]] = '.';
                    perspos[1] = perspos[1]+1;
                    gameover(grid, perspos, step);
                }
                else if(grid[perspos[0]][perspos[1]+1] == 'X' && countones(collhealth) != 0)
                {
                    usehealthsupply(collhealth);
                    //move to that cell
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]][perspos[1]+1] = 'o';
                    valid = 1;
                    cured = 1;
                    findperson(grid, perspos);
                    printover(grid, perspos);
                    cout<<"The \u26C4 is infected, but it can use a \u2764 to recover."<<endl;
                }
                else if(grid[perspos[0]][perspos[1]+1] == '|')
                {
                    cout<<endl<<"The \u26C4 can't go there, as that person is in self-isolation."<<endl;
                }
                else if(grid[perspos[0]][perspos[1]+1] == 'h')
                {
                    collhealth[perspos[0]][perspos[1]+1] = 1;
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]][perspos[1]+1] = 'o';
                    cout<<endl<<"You've collected a \u2764!"<<endl;
                    valid = 1;
                }
                else
                {
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]][perspos[1]+1] = 'o';
                    valid = 1;
                }
                break;
            case 'u':
                if(checkvalidmove(perspos[0]-1) == 1)
                   break;
                else if(grid[perspos[0]-1][perspos[1]] == 'X' && countones(collhealth) == 0)
                {
                    grid[perspos[0]][perspos[1]] = '.';
                    perspos[0] = perspos[0]-1;
                    gameover(grid, perspos, step);
                }
                else if(grid[perspos[0]-1][perspos[1]] == 'X' && countones(collhealth) != 0)
                {
                    usehealthsupply(collhealth);
                    //move to that cell
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]-1][perspos[1]] = 'o';
                    valid = 1;
                    cured = 1;
                    findperson(grid, perspos);
                    printover(grid, perspos);
                    cout<<"The \u26C4 is infected, but it can use a \u2764 to recover."<<endl;
                }
                else if(grid[perspos[0]-1][perspos[1]] == '|')
                {
                    cout<<endl<<"The \u26C4 can't go there, as that person is in self-isolation."<<endl;
                }
                else if(grid[perspos[0]-1][perspos[1]] == 'h')
                {
                    collhealth[perspos[0]-1][perspos[1]] = 1;
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]-1][perspos[1]] = 'o';
                    cout<<endl<<"You've collected a \u2764!"<<endl;
                    valid = 1;
                }
                else
                {
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]-1][perspos[1]] = 'o';
                    valid = 1;
                }
                break;
            case 'd':
                if(checkvalidmove(perspos[0]+1) == 1)
                    break;
                else if(grid[perspos[0]+1][perspos[1]] == 'X' && countones(collhealth) == 0)
                {
                    grid[perspos[0]][perspos[1]] = '.';
                    perspos[0] = perspos[0]+1;
                    gameover(grid, perspos, step);
                }
                else if(grid[perspos[0]+1][perspos[1]] == 'X' && countones(collhealth) != 0)
                {
                    usehealthsupply(collhealth);
                    //move to that cell
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]+1][perspos[1]] = 'o';
                    valid = 1;
                    cured = 1;
                    findperson(grid, perspos);
                    printover(grid, perspos);
                    cout<<"The \u26C4 is infected, but it can use a \u2764 to recover."<<endl;
                }
                else if(grid[perspos[0]+1][perspos[1]] == '|')
                {
                    cout<<endl<<"The \u26C4 can't go there, as that person is in self-isolation."<<endl;
                }
                else if(grid[perspos[0]+1][perspos[1]] == 'h')
                {
                    collhealth[perspos[0]+1][perspos[1]] = 1;
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]+1][perspos[1]] = 'o';
                    cout<<endl<<"You've collected a \u2764!"<<endl;
                    valid = 1;
                }
                else
                {
                    grid[perspos[0]][perspos[1]] = '.';
                    grid[perspos[0]+1][perspos[1]] = 'o';
                    valid = 1;
                }
                break;
            //stay in the same position means we don't need to check if valid or if infected
            case 's':
                //check whether that cell was previously infected, and so if by staying in it (without health supplies) the snowman gets infected
                if(grid[perspos[0]][perspos[1]] == 'X' && countones(collhealth) == 0)
                    gameover(grid, perspos, step);
                else if(grid[perspos[0]][perspos[1]] == 'X' && countones(collhealth) != 0)
                {
                    usehealthsupply(collhealth);
                    cured = 1;
                    printover(grid, perspos);
                    cout<<"The \u26C4 is infected, but it can use a \u2764 to recover."<<endl;
                }
                //staying is always a valid move
                valid = 1;
                break;
            case 'g':
                cout<<endl<<"Better luck next pandemic!"<<endl<<endl;
                exit(0);
            default:
                cout<<endl<<"That's an invalid move, try again."<<endl;
                break;
        }
    }while(valid == 0);
}

void checkwin(char grid[n][n], int perspos[2], int step, int winparam)
{
    findperson(grid, perspos);
    int winflag = 0;
    //if person is at any of the three corners (apart from top left corner), we've won
    //perspos[0] is the row and perspos[1] is the col
    if(winparam == 0 && perspos[0] == 0 && perspos[1] == n-1) //top right corner
        winflag = 1;
    else if(winparam == 1 && perspos[0] == n-1 && perspos[1] == 0) //bottom left corner
        winflag = 1;
    else if(winparam == 2 && perspos[0] == n-1 && perspos[1] == n-1) //bottom right corner
        winflag = 1;
    if(winflag == 1)
    {
        cout<<endl;
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                if(winparam == 0 && i == 0 && j == n-1)
                    cout<<"\u26FA"<<" ";
                else if(winparam == 1 && i == n-1 && j == 0)
                    cout<<"\u26FA"<<" ";
                else if(winparam == 2 && i == n-1 && j == n-1)
                    cout<<"\u26FA"<<" ";
                else if(grid[i][j] == '|')
                    cout<<"\u26F1"<<"  ";
                else if(grid[i][j] == 'h')
                    cout<<"\u2764"<<"  ";
                else
                    cout<<grid[i][j]<<"  ";
            }
            cout<<endl;
        }
        cout<<endl<<"The \u26C4 has made it out of the city in "<<step<<" hours!"<<endl<<"CONGRATULATIONS!"<<endl<<endl;
        exit(0);
    }
}

//check that virus does not start spreading from the position of the portal
int rowcolviruscheck(int winparam, int infno, int randpointrowarr[infno], int randpointcolarr[infno])
{
    int i, flag = 0;
    if(winparam == 0)
    {
        for(i = 0; i < infno; i++)
        {
            if(randpointrowarr[i] == 0 && randpointcolarr[i] == n-1)
                flag = 1;
        }
        //flag = 0 means the virus doesn't start spreading from the top right corner
        if(flag == 0)
            return 1;
        else
            return 0;
    }
    else if(winparam == 1)
    {
        for(i = 0; i < infno; i++)
        {
            if(randpointrowarr[i] == n-1 && randpointcolarr[i] == 0)
                flag = 1;
        }
        //flag = 0 means the virus doesn't start spreading from the bottom left corner
        if(flag == 0)
            return 1;
        else
            return 0;
    }
    else if(winparam == 2)
    {
        for(i = 0; i < infno; i++)
        {
            if(randpointrowarr[i] == n-1 && randpointcolarr[i] == n-1)
                flag = 1;
        }
        //flag = 0 means the virus doesn't start spreading from the bottom right corner
        if(flag == 0)
            return 1;
        else
            return 0;
    }
    //if winparam is anything else, but this shouldn't happen
    else
        return 0;
}

//set location of portal
int setportal(char grid[n][n], int infno, int randpointrowarr[infno], int randpointcolarr[infno])
{
    int winparam, escapeflag = 0;
    //if the portal isn't where the virus starts spreading from (otherwise impossible to win)
    do
    {
        winparam = rand() % 3; //generate random number between 0 and 2
        //if virus doesn't start spreading from the position of the portal
        if(rowcolviruscheck(winparam, infno, randpointrowarr, randpointcolarr) == 1)
        {
            if(winparam == 0)
            {
                cout<<"top right corner!"<<endl;
                escapeflag = 1;
                grid[0][n-1] = 'p';
            }
            else if(winparam == 1)
            {
                cout<<"bottom left corner!"<<endl;
                escapeflag = 1;
                grid[n-1][0] = 'p';
            }
            else if(winparam == 2)
            {
                cout<<"bottom right corner!"<<endl;
                escapeflag = 1;
                grid[n-1][n-1] = 'p';
            }
        }
    }while(escapeflag == 0);
    return winparam;
}

void setpatientzeros(char grid[n][n], int infno, int randpointrowarr[infno], int randpointcolarr[infno])
{
    //to keep track of number of chosen patient zeros
    int randflag = 0, infctr = 0;
    int randpointrow, randpointcol;
    do
    {
        do
        {
            randpointrow = rand() % n; //random number between 0 and n-1
            randpointcol = rand() % n;
            //if the virus isn't at the same cell as the person
            if(randpointrow != 0 || randpointcol != 0)
            {
                grid[randpointrow][randpointcol] = 'X';
                randflag = 1;
                randpointrowarr[infctr] = randpointrow;
                randpointcolarr[infctr] = randpointcol;
                infctr++;
            }
        }while(randflag == 0);
        //infctr starts from 0 and increments every time we store a row/col
        //so this tells us how many patient zeros we have
        //we need to loop until this is equal to the number of patient zeros we need i.e. infno
    }while(infctr < infno);
}

//set positions of social distancing cells
void setsocialdist(char grid[n][n], int socno, int socrowarr[socno], int soccolarr[socno])
{
    int i, socrow, soccol, socflag = 0, socnoctr = 0;
    //sometimes we might have no people social distancing (socno = 0), so we use a while loop
    while(socnoctr < socno)
    {
        do
        {
            //position of social distancing cell
            socrow = rand() % n; //random number between 0 and n-1
            soccol = rand() % n;
            //if the social distancing cell isn't at the same cell as the person
            if(socrow != 0 || soccol != 0)
            {
                //if the social distancing cell isn't at the same cell as the portal
                if(grid[socrow][soccol] != 'p')
                {
                    if(grid[socrow][soccol] != 'X')
                    {
                        grid[socrow][soccol] = '|';
                        socflag = 1;
                        //store positions of social distancing cells in array
                        socrowarr[socnoctr] = socrow;
                        soccolarr[socnoctr] = soccol;
                        socnoctr++;
                    }
                }
            }
        }while(socflag == 0);
    }
}

//set positions of health supplies
void sethealthsupplies(char grid[n][n], int healthno, int healthrowarr[healthno], int healthcolarr[healthno], int collhealth[n][n])
{
    int i, j, k, check, healthrow, healthcol, healthflag = 0, healthnoctr = 0;
    //sometimes we might have no health supplies (healthno = 0), so we use a while loop
    while(healthnoctr < healthno)
    {
        do
        {
            //position of health supply
            healthrow = rand() % n; //random number between 0 and n-1
            healthcol = rand() % n;
            //if the health supply isn't at the same cell as the person
            if(healthrow != 0 || healthcol != 0)
            {
                //if the health supply isn't at the same cell as the portal
                if(grid[healthrow][healthcol] != 'p')
                {
                    //if the health supply isn't at the same cell as a social distancing cell
                    if(grid[healthrow][healthcol] != '|')
                    {
                        if(grid[healthrow][healthcol] != 'X')
                        {
                            grid[healthrow][healthcol] = 'h';
                            healthflag = 1;
                            //store positions of health supply in array
                            healthrowarr[healthnoctr] = healthrow;
                            healthcolarr[healthnoctr] = healthcol;
                            healthnoctr++;
                        }
                    }
                }
            }
        }while(healthflag == 0);
    }
    //collhealth[n][n] is a matrix corresponding to the whole grid
    //keeps track of collected health supplies
    //we start with each element of collhealth[] being -5
    //if there is a health supply at a cell, set this value to 0
    //we set it to 1 if collected, and -1 once used up
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            check = 0;
            //loop through all cells with health supplies
            for(k = 0; k < healthno; k++)
            {
                //if i, j correspond to the row, col of a health supply
                if(i == healthrowarr[k] && j == healthcolarr[k])
                {
                    collhealth[i][j] = 0;
                    check = 1;
                }
            }
            //if the cell doesn't correspond to a cell with a health supply
            if(check == 0)
                collhealth[i][j] = -5;
        }
    }
}

//set all the elements of a grid to .
void initialiseblankgrid(char grid[n][n])
{
    for(int i = 0; i < n; i++)
    {
    for(int j = 0; j < n; j++)
        {
            grid[i][j] = '.';
        }
    }
}

//initialise values of infections matrix for patient zeros
void initialiseinfections(int infections[n][n], int infno, int randpointrowarr[infno], int randpointcolarr[infno])
{
    int i, j, k;
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            for(k = 0; k < infno; k++)
            {
                if(i == randpointrowarr[k] && j == randpointcolarr[k])
                    infections[i][j] = 0;
                else
                    infections[i][j] = -1;
            }
        }
    }
}

int main()
{
    //seed the random number generator
    srand (time(NULL));
    
    //loop variables
    int i, j, k, l;
    
    //original grid
    char grid[n][n];
    
    //new grid after evolution
    char newgrid[n][n];
    
    //initialise blank grid and blank newgrid (which we update after virus spreads)
    initialiseblankgrid(grid);
    initialiseblankgrid(newgrid);
    
    cout<<"\033[2J\033[1;1H"; //clears the screen
    cout<<endl<<"\u24DF \u24D0 \u24DD \u24D3 \u24D4 \u24DC \u24DE \u24DD \u24D8 \u24D2"<<endl<<endl; //outputs PANDEMONIC
    
    cout<<"A virus is spreading across the city."<<endl<<endl<<"The parameter R0 tells us how contagious it is."<<endl<<"eg. R0 = 2 means each person infects (on average) 2 other people."<<endl<<endl;
    
    //R0 is the number of cells each infected cell will infect
    int R0;
    cout<<"Choose the R0 of the virus."<<endl;
    cin>>R0;
    
    //snowman starts at top left corner
    grid[0][0] = 'o';
    
    int perspos[2];
    findperson(grid, perspos);
    
    //infno is the number of infected people in the grid at the beginning
    int infno;
    cout<<endl<<"Choose how many infected people are in the city at \u231B Hour 0."<<endl;
    cin>>infno;
    //to keep track of row and col of patient zeros
    int randpointrowarr[infno], randpointcolarr[infno];
    setpatientzeros(grid, infno, randpointrowarr, randpointcolarr);
    
    //randomly pick the escape route - the portal
    cout<<endl<<"-----------------------------------------------------------------------------"<<endl;
    cout<<endl<<"HOW TO PLAY"<<endl;
    cout<<endl<<"Help the \u26C4 escape from the city by reaching the \u15DD in the ";
    //this picks a random corner to be the location of the portal
    int winparam;
    winparam = setportal(grid, infno, randpointrowarr, randpointcolarr);
    
    cout<<"If there's no escape, try to remain uninfected as long as possible."<<endl;
    cout<<endl<<"Collect \u2764 so the \u26C4 can recover if infected."<<endl;
    cout<<"\u26F1  indicates a person in self-isolation - the virus can't spread to them."<<endl<<endl;
    cout<<"-----------------------------------------------------------------------------"<<endl<<endl;
    cout<<"\u231B Hour 0"<<endl<<" \u2764 Health 0"<<endl<<endl;
    
    //social distancing/self-isolation
    int socno, maxsoc;
    //max number of people social distancing = ceil(n/5) eg. for n = 10, socno = 2
    maxsoc = ceil(n/5);
    //random number between 0 and maxsoc tells us how many people are social distancing
    socno = rand() % (maxsoc+1);
    //arrays to keep track of all socno social distancing people
    int socrowarr[socno], soccolarr[socno];
    setsocialdist(grid, socno, socrowarr, soccolarr);

    //health supplies
    int healthno, maxhealth;
    //max number of health supplies = ceil(n/2) eg. for n = 10, healthno = 5
    maxhealth = ceil(n/2);
    //random number between 1 and maxhealth tells us how many health supplies there are
    healthno = rand() % maxhealth;
    healthno++;
    //arrays to keep track of all healthno social distancing people
    int healthrowarr[healthno], healthcolarr[healthno];
    //collhealth keeps track of collected health supplies
    int collhealth[n][n];
    sethealthsupplies(grid, healthno, healthrowarr, healthcolarr, collhealth);
    
    print(grid);
    
    //elements of this matrix keep track of how many cells each infected cell has infected (max R0)
    int infections[n][n];
    initialiseinfections(infections, infno, randpointrowarr, randpointcolarr);
    
    cout<<endl<<"Watch out, the virus X is spreading..."<<endl<<"Try and get to the \u15DD - or try not to get infected!"<<endl;
    
    //keeps track of whether the snowman was cured after moving into/staying in an infected cell
    int cured;
    
    //keeps track of how many steps of spreading we have
    int step = 0;
    do
    {
        step++;
        cout<<endl;
        //number of dashes is almost equal to n*3
        for(i = 1; i <= n*3; i++)
        {
            cout<<"-";
        }
        cout<<endl<<endl<<"\u231B Hour "<<step<<endl<<" \u2764 Health "<<countones(collhealth)<<endl;
        
        cured = 0;
        
        //move the person according to user input
        findperson(grid, perspos);
        moveperson(grid, perspos, step, collhealth, cured);
        findperson(grid, perspos);
        
        checkwin(grid, perspos, step, winparam);
        infect(grid, newgrid, R0, infections, perspos);
        
        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                grid[i][j] = newgrid[i][j];
            }
        }
        
        findperson(grid, perspos);
        
        //keep track of social distancing cells
        for(i = 0; i < socno; i++)
        {
            grid[socrowarr[i]][soccolarr[i]] = '|';
        }
        
        //keep track of health supplies
        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                //if health supply is available (!= -5) and not collected (!>= 1) i.e. = 0
                if(collhealth[i][j] == 0)
                    grid[i][j] = 'h';
            }
        }
        
        //cured == 0 ensures that it hasn't already been cured in moveperson()
        if(infectedperson(grid, perspos) == 1 && countones(collhealth) == 0 && cured == 0)
            gameover(grid, perspos, step);
        else if(infectedperson(grid, perspos) == 1 && countones(collhealth) != 0 && cured == 0)
        {
            usehealthsupply(collhealth);
            //don't need cured = 1 here because that's just to check if it's been cured in moveperson() before virus spreads
            printover(grid, perspos);
            cout<<"The \u26C4 is infected, but it can use a \u2764 to recover."<<endl;
        }
        
        checkwin(grid, perspos, step, winparam);
        
        //keep track of position of person
        grid[perspos[0]][perspos[1]] = 'o';
        
        //outputs some white space
        cout<<string(1, '\n');
        print(grid);
        //cout<<endl<<"Number of infected cells = "<<count(grid)<<endl;
        
        if(spread(infections, R0) == 1)
            cout<<endl<<"Watch out, the virus X is spreading..."<<endl<<"Try and get to the \u15DD - or try not to get infected!"<<endl;
        else
            cout<<endl<<"The virus has fully spread!"<<endl<<"Get the \u26C4 out of the city if you can."<<endl<<"Press g if you can't reach the \u15DD ... welcome to your new home, \u26C4."<<endl;
        
    //keep going while the person is not infected
    }while(infectedperson(grid, perspos) == 0);

    return 0;
}
