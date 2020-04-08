#include<iostream>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
using namespace std;

//size of grid
const int n = 15;

//elements of this matrix keep track of how many cells each infected cell has infected (max R0)
//int infections[n][n];

//prints character grid
void print(char grid[n][n])
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            cout<<grid[i][j]<<" ";
        }
        cout<<endl;
    }
}

//prints integer grid
void printint(int grid[n][n])
{
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
        {
            cout<<grid[i][j]<<" ";
        }
        cout<<endl;
    }
}

//infect cells of grid
void infect(char grid[n][n], char newgrid[n][n], int R0, int infections[n][n])
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
                    //so that we don't choose a previously chosen neighbour
                    //and so that we don't choose the cell itself
                    //and so that our chosen neighbouring cell lies in the grid
                    }while((flag == 1) || ((c == 0) && (d == 0)) || (i+c >= n) || (i+c < 0) || (j+d >= n) || (j+d < 0));
                    
                    //infect that randomly chosen neighbouring cell
                    //cout<<"cell ["<<i+c<<"]["<<j+d<<"]!"<<endl;
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

//outputs 0 if full spread has occurred => stop
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

int main()
{
    //seed the random number generator
    srand (time(NULL));
    
    int i, j;
    
    //original grid
    char grid[n][n];
    
    //new grid after evolution
    char newgrid[n][n];
    
    //print blank grid
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            grid[i][j] = '.';
        }
    }
    cout<<endl;
    print(grid);
    
    //first infected person - patient zero
    int row, col;
    cout<<endl<<"Enter the coordinates of Patient Zero."<<endl;
    cin>>row>>col;
    grid[row-1][col-1] = 'X';
    cout<<endl;
    print(grid);
    
    //set the updated grid to blank
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            newgrid[i][j] = '.';
        }
    }
    
    //store number of infections from each cell
    int infections[n][n];
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            if(i == row-1 && j == col-1)
            {
                infections[i][j] = 0;
            }
            else
            {
                infections[i][j] = -1;
            }
        }
    }
    
    //R0 is the number of cells each infected cell will infect
    int R0;
    cout<<endl<<"What is the R0 of the virus?"<<endl;
    cin>>R0;
    
    //spread until the whole grid is infected
    cout<<endl<<"All right! The virus begins spreading..."<<endl;

    //keeps track of how many steps of spreading we have
    int step = 0;
    do
    {
        step++;
        cout<<endl<<"In Step "<<step<<":"<<endl;
        
        infect(grid, newgrid, R0, infections);
        
        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                grid[i][j] = newgrid[i][j];
            }
        }
        
        //outputs some white space
        cout<<string(5, '\n');
        print(grid);
        cout<<endl<<"Number of infected cells = "<<count(grid)<<endl;
        
        //freezes screen for 1 sec
        usleep(1000000);
        
    }while(spread(infections, R0) == 1);
    
    cout<<endl<<"It took "<<step<<" steps for the virus to fully spread through a grid of "<<n*n<<" cells."<<endl<<endl;
    
    //printint(infections);

    return 0;
}
