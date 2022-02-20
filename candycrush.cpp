#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<iostream>
#include<sstream>
#include<time.h>
using namespace sf;
using namespace std;

int ts=48;
Vector2i offset(0,0);
Vector2i offset1(10,7);
Vector2i offset2(400,0);

struct piece
{
    int x,y,col,row,kind,match;
    piece()
    {
        match=0;
    }
}grid[10][10];

void Swap(piece p1,piece p2)
{
    std::swap(p1.col,p2.col);
    std::swap(p1.row,p2.row);
    grid[p1.row][p1.col]=p1;
    grid[p2.row][p2.col]=p2;
}

int ns=10;
int s=0;

int main()
{ 
    srand(time(0));

    RenderWindow app(VideoMode(700,400), "Match-3 Game!");
    app.setFramerateLimit(60);

    SoundBuffer buffer;
    buffer.loadFromFile("candygame/mixkit-hard-pop-click-2364.wav");
    
    Sound sound;
    sound.setBuffer(buffer);

    Texture tb,tg;
    tb.loadFromFile("candygame/Board.png");
    tg.loadFromFile("candygame/bubbles.png");
    
    Sprite backgroud(tb) , gems(tg);
    //initialising the grid.
    for(int i=1;i<9;i++)
    {
        for(int j=1;j<9;j++)
        {
            grid[i][j].kind=rand()%7;
            grid[i][j].col=j;
            grid[i][j].row=i;
            grid[i][j].x=j*ts;
            grid[i][j].y=i*ts;
        }
    } 

    int x0,y0,x,y , click=0;
    Vector2i pos;
    bool isSwap=false,ismoving= false;

    while (app.isOpen() && ns>0)
    {
        Event e;
        //recognixing the mouse clicks
        while(app.pollEvent(e))
        {
            if(e.type == Event::Closed)
            {
                app.close();
            }

            if(e.type ==Event::MouseButtonPressed)
            {
                if(e.mouseButton.button == Mouse::Left)
                {
                    if(!isSwap && !ismoving)
                    {
                        click++;
                        sound.play();
                    }
                    pos=Mouse::getPosition(app)-offset1;
                }
            }
        }

        //finding the elements of the grid which the mouse has clicked.
        if(click==1)
        {
            x0=pos.x/ts+1;
            y0=pos.y/ts+1;
        }
        if(click==2)
        {
            ns--;
            x=pos.x/ts+1;
            y=pos.y/ts+1;
            if(abs(x-x0)+abs(y-y0)==1)
            {
                Swap(grid[y0][x0],grid[y][x]);
                isSwap=1;
                click=0;
            }
            else
            {
                click=1;
            }           
        }
            
        //match finding.
        for(int i=1;i<9;i++)
        {
            for(int j=1;j<9;j++)
            {

                if(grid[i][j].kind==grid[i+1][j].kind && grid[i][j].kind==grid[i-1][j].kind )
                {
                    grid[i-1][j].match++;
                    grid[i][j].match++;
                    grid[i+1][j].match++;
                }
                if(grid[i][j].kind==grid[i][j+1].kind && grid[i][j].kind==grid[i][j-1].kind)
                {
                    grid[i][j-1].match++;
                    grid[i][j].match++;
                    grid[i][j+1].match++;
                }
            }
        }

        //moving animation
        ismoving=false;
        for(int i=1;i<9;i++)
        {
            for(int j=1;j<9;j++)
            {
                piece &p =grid[i][j];
                int dx,dy;
                for(int n=0;n<4;n++)
                {
                    dx=p.x-p.col*ts ,dy= p.y -p.row*ts;
                    if(dx)
                    {
                        p.x -= dx/abs(dx);
                    }
                    if(dy)
                    {
                        p.y -= dy/abs(dy);
                    }
                    if(dx || dy)
                    {
                        ismoving=1;
                    }
                }
            }
        }
        
        //score counting
        int score =0;
        for(int i=1;i<9;i++)
        {
            for(int j=1;j<9;j++)
            {
                score += grid[i][j].match;
                if(grid[i][j].match)
                {
                    s++;
                }
            }
        }
        //second swap if no matching.
        if(isSwap && !ismoving)
        {
            if(!score)
            {
                Swap(grid[y0][x0],grid[y][x]);
            }
            isSwap=0;
        }
        
        //updating the grid
        if(!ismoving)
        {
            for(int i=8;i>0;i--)
            {
                for(int j=1;j<9;j++)
                {
                    if(grid[i][j].match)
                    {
                        for(int n=i;n>0;n--)
                        {
                            if(!grid[n][j].match)
                            {
                                Swap(grid[n][j] , grid[i][j]);
                                break;
                            }
                        }
                    }
                }
            }

            for(int j=1;j<=8;j++)
            {
                for(int i=8 , n=0;i>0;i--)
                {
                    if(grid[i][j].match)
                    {
                        grid[i][j].kind=rand()%7;
                        grid[i][j].y = -ts*(n++);
                        grid[i][j].match=0;
                    }
                }
            }
        }
        
        //Printing the score and remaining moves.
        app.clear();
        stringstream rr ;
        rr <<"TARGET:1200" << endl << endl << "Remaining moves" << endl << ns << endl << endl << endl << "Score" << endl << s; 
        Text Rm;
        Rm.setPosition(450,75);
        Vector2f position =Rm.getPosition();
        Font font;
        font.loadFromFile("candygame/YBTallPretty.ttf");
        Rm.setFont(font);
        Rm.setFillColor(Color::White);
        Rm.setString(rr.str());
        app.draw(Rm);

        //drawing the background.
        backgroud.setPosition(offset.x,offset.y);
        app.draw(backgroud);

        //drawing the candies.
        for(int i=1;i<9;i++)
        {
            for(int j=1;j<9;j++)
            {
                piece p=grid[i][j];
                gems.setTextureRect( IntRect(p.kind*49,0,49,49));
                gems.setPosition(p.x,p.y);
                gems.move(offset1.x-ts,offset1.y-ts);
                app.draw(gems);
            }
        }

        // Show the window
        app.display();
    }
    RenderWindow Window(VideoMode(400,400), "Result");
    Window.setFramerateLimit(60);
    Texture t;
    t.loadFromFile("candygame/Board.png");
    Sprite background(t);
    while (Window.isOpen())
    {   
        background.setPosition(0,0);
        Window.draw(background);     
        stringstream res;
        if(s>=1200)
        {
            res << "Target:1200" << endl << "Your Score"<<endl << s << endl << "YOU WON" << endl<<endl<<endl<<"Press esc to quit" ;
        }
        else
        {
            res << "Target:1200" << endl << "Your Score"<<endl << s << endl << "YOU LOST"<< endl<<endl<<endl<<"Press esc to quit" ;
        }
        Text r;
        r.setPosition(100,150);
        Font font;
        font.loadFromFile("candygame/YBTallPretty.ttf");
        r.setFont(font);
        r.setString(res.str());
        Window.draw(r);

        Event e;
        while(Window.pollEvent(e))
        {
            if(e.key.code==Keyboard::Escape)
            {
                Window.close();
            }
        }

        Window.display();
    }
    return 0;
}
