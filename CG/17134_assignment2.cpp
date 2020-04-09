#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#define maxHt 800
#define maxWd 600
#define maxVer 10000

//float allcases_x[63]={0, 10, 50, 80, 100, 110, 110, 130, 130, 100, 130, 130, 110, 80, 70, 70, 80, 70, 60, 40, 30, 10,80,90,100,130,140,160,150,160,150,155,145,140,140,130,110,90,80,80,90,90,80,80,90,120,100,100,120,110,100,90,87.5,90,96,101,96,90,125.0,375.0,150,250,350};
//float allcases_y[63]={30, 10, 10, 40, 40, 50, 115, 120, 130, 135, 140, 150, 160, 160, 150, 130, 120, 110, 60, 30, 20, 20,100,90,90,80,90,90,95,100,100,110,100,110,100,90,110,110,60,40,30,20,10,5,0,0,10,20,40,60,70,70,150,147,147,150,152.5,152.5,275,275,150,350,150};

float case1_x[22] = {0, 10, 50, 80, 100, 110, 110, 130, 130, 100, 130, 130, 110, 80, 70, 70, 80, 70, 60, 40, 30, 10};
float case1_y[22] = {30, 10, 10, 40, 40, 50, 115, 120, 130, 135, 140, 150, 160, 160, 150, 130, 120, 110, 60, 30, 20, 20};
float case2_x[16]={80,90,100,130,140,160,150,160,150,155,145,140,140,130,110,90};
float case2_y[16]={100,90,90,80,90,90,95,100,100,110,100,110,100,90,110,110};
float case3_x[14]={80,80,90,90,80,80,90,120,100,100,120,110,100,90};
float case3_y[14]={60,40,30,20,10,5,0,0,10,20,40,60,70,70};
float case4_x[6]={87.5,90,96,101,96,90};
float case4_y[6]={150,147,147,150,152.5,152.5};
float case5_x[5]={125.0,375.0,150,250,350};
float case5_y[5]={275,275,150,350,150};

typedef struct edgebucket
{
    int ymax;      //maximum of y coordinate of edge
    float xofymin; //x coordinate of lowest edge
    float slopeinverse;
} Edgetable;

typedef struct edgetabletup
{
    int countEdgeBucket;
    Edgetable buckets[maxVer];
} EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;

void initEdgeTable()
{
    int i;
    for (i = 0; i < maxHt; i++)
    {
        EdgeTable[i].countEdgeBucket = 0;
    }

    ActiveEdgeTuple.countEdgeBucket = 0;
}

void printTuple(EdgeTableTuple *tup)
{
    int j;

    if (tup->countEdgeBucket)

    for (j = 0; j < tup->countEdgeBucket; j++)
    {
        printf("(%d :%.2f,%.2f\n",
                tup->buckets[j].ymax, tup->buckets[j].xofymin, tup->buckets[j].slopeinverse);
    }
}

void printTable()
{
    int i, j;

    for (i = 0; i < maxHt; i++)
    {
        if (EdgeTable[i].countEdgeBucket)
        printTuple(&EdgeTable[i]);
    }
}

void sortTables(EdgeTableTuple *ett) //sort the points in list
{
    int i, j;
    Edgetable temp;

    for (i = 1; i < ett->countEdgeBucket; i++)
    {
        temp.ymax = ett->buckets[i].ymax;
        temp.xofymin = ett->buckets[i].xofymin;
        temp.slopeinverse = ett->buckets[i].slopeinverse;
        j = i - 1;

        while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0))
        {
            ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
            ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
            ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
            j = j - 1;
        }
        ett->buckets[j + 1].ymax = temp.ymax;
        ett->buckets[j + 1].xofymin = temp.xofymin;
        ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
    }
}

void storeEdgeInTuple(EdgeTableTuple *receiver, int ym, int xm, float slopInv)
{
    (receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
    (receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
    (receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;

    sortTables(receiver);

    (receiver->countEdgeBucket)++;
}

void storeEdgeInTable(int x1, int y1, int x2, int y2)
{
    float m, minv;
    int ymaxTS, xwithyminTS, scanline; 

    if (x2 == x1)
    {
        minv = 0.000000;
    }
    else
    {
        m = ((float)(y2 - y1)) / ((float)(x2 - x1));

        if (y2 == y1)
            return;

        minv = (float)1.0 / m;
//        printf("\nSlope string for %d %d & %d %d: %f", x1, y1, x2, y2, minv);
    }

    if (y1 > y2)
    {
        scanline = y2;
        ymaxTS = y1;
        xwithyminTS = x2;
    }
    else
    {
        scanline = y1;
        ymaxTS = y2;
        xwithyminTS = x1;
    }
    storeEdgeInTuple(&EdgeTable[scanline], ymaxTS, xwithyminTS, minv);
}

void removeEdgeByYmax(EdgeTableTuple *Tup, int yy)
{
    int i, j;
    for (i = 0; i < Tup->countEdgeBucket; i++)
    {
        if (Tup->buckets[i].ymax == yy)
        {
            for (j = i; j < Tup->countEdgeBucket - 1; j++)
            {
                Tup->buckets[j].ymax = Tup->buckets[j + 1].ymax;
                Tup->buckets[j].xofymin = Tup->buckets[j + 1].xofymin;
                Tup->buckets[j].slopeinverse = Tup->buckets[j + 1].slopeinverse;
            }
            Tup->countEdgeBucket--;
            i--;
        }
    }
}

void updatexbyslopeinv(EdgeTableTuple *Tup)
{
    int i;

    for (i = 0; i < Tup->countEdgeBucket; i++)
    {
        (Tup->buckets[i]).xofymin = (Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
    }
}

void ScanlineFill()
{
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
    for (i = 0; i < maxHt; i++) 
    {
        for (j = 0; j < EdgeTable[i].countEdgeBucket; j++)
        {
            storeEdgeInTuple(&ActiveEdgeTuple, EdgeTable[i].buckets[j].ymax, EdgeTable[i].buckets[j].xofymin,
                             EdgeTable[i].buckets[j].slopeinverse);
        }
//        printTuple(&ActiveEdgeTuple);
        removeEdgeByYmax(&ActiveEdgeTuple, i);
        sortTables(&ActiveEdgeTuple);
        printTuple(&ActiveEdgeTuple);
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;
        while (j < ActiveEdgeTuple.countEdgeBucket)
        {
            if (coordCount % 2 == 0)
            {
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1 == x2)
                {
                    if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
                    {
                        x2 = x1;
                        ymax2 = ymax1;
                    }

                    else
                    {
                        coordCount++;
                    }
                }

                else
                {
                    coordCount++;
                }
            }
            else
            {
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;
                FillFlag = 0;
                if (x1 == x2)
                {
                    if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
                    {
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else
                    {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else
                {
                    coordCount++;
                    FillFlag = 1;
                }

                if (FillFlag)
                {
                    glColor3f(0.0f, 0.7f, 0.0f);
                    glBegin(GL_LINES);
                    glVertex2i(x1, i);
                    glVertex2i(x2, i);
                    glEnd();
                    glFlush();
                }
            }

            j++;
        }

        updatexbyslopeinv(&ActiveEdgeTuple);
    }
}

void myInit(void)
{

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluOrtho2D(-400, maxHt, -400, maxWd);   //-400 for adjusting the image to center coz. max height and width
    glClear(GL_COLOR_BUFFER_BIT);
}

void drawShapesPoint(float arr_x[],float arr_y[], float r, float g, float b,int n)
{

    glColor3f(r,g,b);
    int count = 0, x1, y1, x2, y2;
    for(int i=0;i<=n;i++)
    {
        count++;
        if (count > 2)
        {
            x1 = x2;
            y1 = y2;
            count = 2;
        }
        if (count == 1)
        {
            x1=arr_x[i];
            y1=arr_y[i];
        }
        else
        {
            x2=arr_x[i];
            y2=arr_y[i];
            glBegin(GL_LINES);
            glVertex2i(x1, y1);
            glVertex2i(x2, y2);
            glEnd();
            storeEdgeInTable(x1, y1, x2, y2); //storage of edges in edge table.

            glFlush();
        }
        
    }
}

void drawShapes(void)
{
    initEdgeTable();
/*    drawShapesPoint(case1_x,case1_y,0,0.5,0,22);
    drawShapesPoint(case2_x,case2_y,1,1,0,16);
    drawShapesPoint(case3_x,case3_y,0.67,0.31,0,14);
    drawShapesPoint(case4_x,case4_y,1,0,0,6);
*/    drawShapesPoint(case5_x,case5_y,1,1,0,5);

    printTable();

    ScanlineFill(); //actual calling of scanline filling..
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(maxHt, maxWd);
    glutInitWindowPosition(100, 150);
    glutCreateWindow("17134_polygon fill algorithm");
    myInit();
    glutDisplayFunc(drawShapes);

    glutMainLoop();
    return 1;
}
