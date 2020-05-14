//11. Определить маршрут, минимизирующий общий километраж и проходящий хотя бы один раз по каждой из улиц при доставке молока.

/*
Алгоритм:
1)	Так как надо найти определенный маршрут, условимся, что точкой начала всегда будет вершина 1 (скажем так, точка отправления доставщика молока, куда по итогу он должен вернуться)
2)	Граф задан матрицей весов, после ввода матрицы, выполняется проверка на четность степеней вершин (для этого достаточно сложить матрицу построчно)
3)	Я рассмотрела 3 случая (исключается случай не связного графа):
a.	В первом случае граф содержит только вершины с четной степенью, это значит, что граф содержит Эйлеров цикл. В этом случае маршрут определяется с помощью алгоритма Флёри. Начиная с некоторой вершины, идем по ребрам, вычеркивая уже пройденные. В конце концов возвращаемся к вершине из которой вышли. Так как вся работа алгоритма сводится к обходу ребер, его временная сложность равна О(Е).
b.	Во втором случае граф содержит 2 вершины с нечетными степенями, это значит, что в графе есть Эйлеров путь. Но т.к. нам нужно вернуться в исходное положение, притом пройти как можно меньше, придется удвоить кратчайший путь между этими вершинами, т.е. получить своего рода Эйлеров цикл с мнимым ребром. Для поиска кратчайшего пути использовала алгоритм Форда, который добавляет ко времени работы еще O(V*E) (количество вершин на количество ребер). Далее найденный путь отправляется в метод way, который проходит по пути, удваивая вершины в матрице смежности. Затем также проходим по ребрам, отсеивая пройденные, учитывая, что есть удвоенные ребра.
c.	В третьем случае граф содержит более 2 вершин с нечетными степенями. В этом случае нужно найти все такие вершины. Сначала с помощью алгоритма Форда находим все кратчайшие пути для найденных вершин, затем составляем из них пары, сумма кратчайших путей между которыми дает минимальный вес. После этого выбранные пути отправляем в метод way, который удваивает вершины и т.д.
4)	Параллельно с обходом ребер, весь путь записывался в отдельный массив, который выводим.
*/
#include <iostream> 
#include <fstream>
#include <time.h>
#include <locale.h>
#include <windows.h>
#include <conio.h>
#include <cstdlib> 
#include <vector> 
#define INF 10000
using namespace std;
//КЛАСС ГРАФ
class Graph
{ private:
     int** matrix_smezh;//матрица смежности для частного случая
    int** matrix;    //матрица весов
    vector < int > cycle_or_way;//цикл или путь
    bool* visitedElements;  //СПИСОК ПОСЕЩЕННЫХ ЭЛЕМЕНТОВ
    int * crat;//массив кратностей вершин
    int count;  // КОЛИЧЕСТВО ВЕРШИН ГРАФА 
    public:
    //существует цикл или только путь или вообще ничего 
    void cycle()
    {
     for (int i = 0; i<count; i++)   //определяем количество ребер для каждой вершины
        for (int j = 0; j < count; j++)
        { if (matrix[i][j])
                ++crat[i];      }
    int coun_not=0; //количество нечетных
    for (int i = 0; i < count; i++)
        if (crat[i] == 0) 
        { cout << "Ошибка, граф не связный" << endl;
            return;    }
        else  if (crat[i] % 2 != 0) 
        {   ++coun_not; 
            crat[i] = 1;//отмечаем нечетные
        } else crat[i] = 0;
    if (coun_not)
    {   cout << "Нет Эйлерова цикла" << endl;
        not_euler(coun_not);   
    }
    else
    {
        cout << "Эйлеров цикл" << endl;
        search_euler(0);
    }
    cout << "ИТОГОВЫЙ МАРШРУТ" << endl;
    for (int i = 0; i < cycle_or_way.size(); i++)
        cout << cycle_or_way[i] << " ";
      }
    void search_euler(int v)//нахождение Эйлерового цикла
    {
        for (int i = 0; i < count; ++i)
            if (matrix[v][i]) // если ребро есть
            {
                matrix[v][i] = 0;// проходим по нему
                matrix[i][v] = 0;
                search_euler(i);
            }
        cycle_or_way.push_back(v+1);
    }
    void search_noteuler(int v)//преобразованный поиск цикла для частного случая
    {
             for (int i = 0; i < count; ++i)
            if (matrix_smezh[v][i]==2) // сначала проходим через двойные ребра, чтоб не пропустить ребро(см. примеры тестов)
            {   matrix_smezh[v][i]-=1;
                matrix_smezh[i][v] = matrix_smezh[v][i];
                search_noteuler(i);      }
         for (int i = 0; i < count; ++i)
            if (matrix_smezh[v][i]) // если ребро есть
            {   matrix_smezh[v][i] = 0; // проходим по нему
                matrix_smezh[i][v] = 0;
                search_noteuler(i);
            }
        cycle_or_way.push_back(v + 1);
    }
    //ЕСЛИ ЕСТЬ НЕЧЕТНЫЕ ВЕРШИНЫ
    void not_euler(int c)
    {   int *nech; //здесь хранятся номера нечетных вершин
        int o = 0;
        if (c == 2)//частный случай - существует Эйлеров путь
        {    nech = new int [2];
            for (int i = 0; i < count; i++)
            {
                if (crat[i])
                    nech[o++] = i;}
            int* h;
            h = bellman_ford(nech[0]);
             way(h, nech[0], nech[1]);
               search_noteuler(0); 
                }
        else//частный случай - нечетных вершин больше 2
        {    nech = new int[c];//массив вершин с нечетными степенями
            for (int i = 0; i < count; i++)
            {
                if (crat[i])
                    nech[o++] = i;
            }
            int** h = new int*[c]; //массив расстояний для восстановления путей
            for (int i = 0; i < c; i++)
                   h[i] = bellman_ford(nech[i]);
          //  cout << endl;
            int **pokr = new int* [c]; //для нахождения пар
            for (int i = 0; i < c; i++)
                pokr[i]= new int[c];
            int k;
            int* nech2 = new int[c];
            for (int i = 0; i < c; i++)
            {   k = 0;
                for (int j = 0; j < count; j++)           
                    if (crat[j])
                     pokr[i][k++] = h[i][j];//заполняем только вершинами с нечетной степенью
                   nech2[i] = nech[i];
            }
            vector<pair <pair <int, int>, int> > min;//поиск пар, дающих минимальный вес
            for (int i = 0; i < c / 2; i++)
            {
                min.push_back(make_pair(make_pair(0, 0), INF));
                for (int t = 0; t < c - 1; t++)
                    for (int j = t + 1; j < c; j++)
                        if (nech2[t] >= 0 && nech2[j] >= 0)
                            if (pokr[t][j] < min[i].second)
                            {
                                min[i].second = pokr[t][j];
                                min[i].first.first = t;
                                min[i].first.second = j;
                            }
                nech2[min[i].first.first] = -1;
                nech2[min[i].first.second] = -1;
            }
            for (int j = 0; j < c / 2; j++)
            {   //    cout << min[j].second << " ";
                way(h[min[j].first.first], nech[min[j].first.first], nech[min[j].first.second]);
             }
                search_noteuler(0);
        }
    }
 
    int* bellman_ford(int s) //алгоритм Форда для поиска кратчайших путей
    {    int i, j, k;
        int* d = new int[count];
        for (i = 0; i < count; i++)
        d[i] = INF;
         d[s] = 0;
         for (k = 0; k < count - 1; k++)//пускаем цикл
               for (i = 0; i < count; i++)// перебираем все ребра
                for (j = 0; j < count; j++)
                   if (matrix[i][j] && d[i] + matrix[i][j] < d[j]) //сравниваем два возможных пути, если они есть
                     d[j] = d[i] + matrix[i][j];
        return d;  }

    void way(int *d, int s, int o)//ВОССТАНОВЛЕНИЕ КРАТЧАЙШИХ ПУТЕЙ
    {
        if (d[o] == INF)
            cout << "Пути не существует" << endl;
        else {
             int big = d[o];
            int* way = new int[count];
            int j = o, k = 1;
            way[0] = j + 1;
            while (big != 0)
            {for (int i = 0; i < count; i++)
                { if (matrix[i][j])
                    { for (int h = 0; h < count; h++)
                        {if (d[h] == big - matrix[i][j])
                            { big = big - matrix[i][j];
                                j = i;
                                way[k++] = j + 1;
                                i = count;
                                break;
                            }   }  }  }   }
      
            for (int i = 0; i < k-1; i++)//добавляем дополнительные ребра
            {   matrix_smezh[way[i + 1]-1][way[i]-1]+= 1;
                matrix_smezh[way[i]-1][way[i + 1]-1] = matrix_smezh[way[i + 1]-1][way[i]-1];
              }    }  }
      //СОЗДАНИЕ ГРАФА
    void GraphCreate(int size)
    {    count = size;
        //создание матрицы смежности
        matrix_smezh = new int* [count];
        for (int i = 0; i < count; i++)
     matrix_smezh[i] = new int[count];
        matrix = new int* [count];
        for (int i = 0; i < count; i++)
            matrix[i] = new int[count];
        visitedElements = new bool[count];
        for (int i = 0; i < count; i++)
             visitedElements[i] = false;
           crat = new int[count];
        for (int i = 0; i < count; i++)
              crat[i] = 0;   }
     //ВЫВОД МАТРИЦЫ
     void printMatrix()
    {  cout << "\t";
        for (int i = 0; i < count; i++)
           cout << i + 1 << "\t";
          cout << endl;
        for (int i = 0; i < count; i++)
        {
            cout << i + 1 << "\t";
            for (int j = 0; j < count; j++)
                cout << matrix[i][j] << "\t";
                cout << endl;
        }   }

    //ВВОД ИЛИ ИЗМЕНЕНИЕ МАТРИЦЫ СМЕЖНОСТИ
    void NewMatrix()
    {     cout << "\nМатрица сейчас:\n";
        printMatrix();
        int a = 1, b = 2, m = 0;
        do {
            fflush(stdin);
            cout << "Для изменения значения ячейки, введите номер строки и номер столбца и значение отличное от нуля через пробел(убрать вес - -10000): " << endl;
            do {
                if (a<1 || a>count || b<1 || b>count || a == b || m < 0 || m >= 10000)
                    cout << "Не верный ввод" << endl;
                cin >> a >> b >> m;
            } while (a<1 || a>count || b<1 || b>count || a == b || m < 0 || m >= 10000);
            fflush(stdin);
            //меняем значение выбранной ячейки
            matrix[a - 1][b - 1] = m;
            matrix[b - 1][a - 1] = m;
            matrix_smezh[b - 1][a - 1] = matrix_smezh[a - 1][b - 1] = 1;
            cout << "\nМатрица:\n";
            printMatrix();
            cout << "1)Ввести еще один пункт(клавиша 1)\n2)Закончить ввод(любая другая клавиша)\n";
        } while (_getch() == '1');
    }
    //ОБНУЛЕНИЕ МАТРИЦы СМЕЖНОСТИ
    void NUllMatrix()
    {
        for (int i = 0; i < count; i++)
        {   for (int j = 0; j < count; j++)
            {   matrix[i][j] = 0;
                matrix_smezh[i][j] = 0;
            }      }  }
    //РАНДОМНО СОБИРАЕМ МАТРИЦУ
       void RondomMatrix()
    {  srand(static_cast<unsigned int>(time(0)));
        for (int i = 0; i < count - 1; i++)
        { for (int j = i + 1; j < count; j++)
            { if (i == j)
                matrix[i][j] = 0;
                 else
                {
                    int number = rand();
                    if (number % 2 == 0)
                    {
                        matrix[i][j] = 1+rand() % 51;
                        matrix[j][i] = matrix[i][j];
                        matrix_smezh[j][i] = matrix_smezh[i][j]=1;
                    }
                    else
                    {
                        matrix[i][j] = 0;
                        matrix[j][i] = 0;
                        matrix_smezh[j][i] = matrix_smezh[i][j] = 0;
                    }          }          }        }    }
     //ВВОД ГРАФА
       int scanfMatr()
    {   cout << "\nВВЕДИТЕ КОЛИЧЕСТВО ЭЛЕМЕНТОВ В ГРАФЕ: ";
        int flag = 0;//Проверка правильного ввода
        do {
            if (flag == 1)
            {
                cout << "Колличество должно быть не более 100 и не менее 2:";
                flag = 0;
            }

            cin >> count;
            if (count <= 0 || count > 100)
                flag = 1;
        } while (flag == 1);
        GraphCreate(count);
        cout << "\nКАК ВВЕСТИ МАТРИЦУ СМЕЖНОСТИ:" << endl;
        cout << "1)Ввод вручную(клавиша 1)\n2)Рандомный ввод матрицы (клавиша 2)\n3) Выход(любая другая клавиша) \n";
        switch (_getch())
        {
        case '1': //Ввод вручную
        {
            fflush(stdin);
            NUllMatrix();
            NewMatrix();
            //cout << "\nИтог:" << endl;
            printMatrix();
              break;
        }
        case '2': //рондомайзим
        {
            fflush(stdin);
           // cout << "\nМатр:" << endl;
            NUllMatrix();
            RondomMatrix();
            printMatrix();
            break;
        }
        default:
        {
            fflush(stdin);
           // NUllMatrix();
            cout << "\nМатрица:" << endl;
            printMatrix();
        }     }
        return 0;
    }
    //КРАСИВЫЙ ВЫВОД ГРАФА
       void PrintGraph()
    {
        ofstream file("graph.dot");
        file << "graph G {" << endl;
        for (int i = 0; i < count; i++)
        {
            file << i + 1 << ";" << endl;
        }
        for (int i = 0; i < count - 1; i++)
        { for (int j = i + 1; j < count; j++)
            {
                   if (matrix[i][j])
            file<<"\"" << i + 1 << "\" -- \"" << j + 1 << "\"[label = \"" << matrix[i][j] << "\"] ;" << endl;
            }        }
        file << "}" << endl;
        file.close();
        system("dot -Tpng graph.dot -o graph.png");
        system("graph.png");
    }};

int main()
{   setlocale(LC_ALL, "RUS");
    Graph f;
    f.scanfMatr();
    f.PrintGraph();
   f.cycle();
}

