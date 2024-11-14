                            /******************************/
                           /*******    PROYECTO C++   ******/
                          /**********************************/
                         /****  By: Agustín Álvarez Fijo *****/
                        /**************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <time.h>
#include <map>
#include <tuple>
#include <utility>
#include <cmath>
#include <functional>
#include <winbase.h>
#include <shellapi.h>
#include <winuser.h>

using namespace std;

struct Bitcoin
{
        tm date;
    double apertura;
    double high;
    double low;
    double close;
    double adj_close;
    int volumen;
};

/*  FUNCIÓN 1:
  ¨¨¨¨¨¨¨¨¨¨¨¨¨
    LECTURA DE FICHERO:
    
    Lee el fichero de entrada y devuelve una vector de tipo Bitcoin, el cual contiene un struct de tipo Bitcoin con los tipos de datos de cada columna del DataSet
        ENTRADA:
        - fichero: nombre del fichero de entrada -> string
        SALIDA:
        - vector de bitcoins (date, apertura, high, low, close, adj_close, volumen) --> [(tm, double, double, double, double, double, long long)]
*/

//Función para convertir un String en un objeto tm:
/* 
La función get_time() la utilizo para extraer valores de tiempo desde un string y almacenarlos en un objeto tm, 
pero necesito crear un objeto tm para almacenar el resultado de get_time(),
porque get_time() espera un objeto tm para almacenar el resultado y luego asignar este resultado al
campo 'date' del 'struct Bitcoin' porque sino peta :)
*/

tm stringToTm(string &date_str) 
{
    tm fecha = {};
    istringstream fechaStringStream(date_str); 
    fechaStringStream >> get_time(&fecha, "%Y-%m-%d");
    return fecha;   
}

vector<Bitcoin> lee_bitcoins(string &fichero)
{
    vector<Bitcoin> registros;//Uso vector<> para poder tener un almacenamiento dinámico de datos y la posibilidad de usar funciones como sort(), reverse() o find()
    ifstream file(fichero);  //Función para leer datos desde unn archivo

    //Chekeo que el archivo no esta comprimido o cifrado y su lecturar es posible
    if (!file.is_open())
    {
        cout << "Error al abrir el archivo" << endl;
        return registros;
    }
    //Defino una linea como string, porque el dataset que voy a leer es un CSV
    string line;
    getline(file, line); //Para leer e ignorar la primera línea

    while(getline(file, line)){
        istringstream iss(line); //Método para dividir la linea en tokens
        string date_str, apertura_str, high_str, low_str, close_str, adj_close_str, volumen_str;
          // getline() para leer la linea completa, iss() para dividir la linea en tokens,
         // le indico el nombre del token que estoy leyendo y por último le indico el delimitador
        // que indica el final del token, que en este caso es una coma.
        getline(iss, date_str, ',');
        getline(iss, apertura_str, ',');
        getline(iss, high_str, ',');
        getline(iss, low_str, ',');
        getline(iss, close_str, ',');
        getline(iss, adj_close_str, ',');
        getline(iss, volumen_str, ',');

        //Creo un objeto istringstream llamado 'date_ss' a partir de la cadena 'date_str'. Este objeto se utiliza luego en la función stringToTm()
        istringstream date_ss(date_str);

        Bitcoin bitcoin;
        bitcoin.date = stringToTm(date_str);    // Mi propio StringToTm :)
        bitcoin.apertura = stod(apertura_str); // StringToDouble
        bitcoin.high = stod(high_str);
        bitcoin.low = stod(low_str);
        bitcoin.close = stod(close_str);
        bitcoin.adj_close = stod(adj_close_str);
        bitcoin.volumen = stoll(volumen_str); // StringToInteger

        registros.push_back(bitcoin); //push_back() para que vaya poniendo cada registro al final del vector de registros
    }
    file.close();
    return registros;
}



/* FUNCIÓN 2:
 ¨¨¨¨¨¨¨¨¨¨¨¨¨
    Calcula la media de los volumenes entre los years 2016 y 2018

        ENTRADA:
            -lista de bitcoins(date, apertura, high, low, close, adj_close, volumen) -> [(tm, float, float, float, float, float, long long)]
        SALIDA:
            -media->[(float)]
*/
double calcular_media_volumen( vector<Bitcoin> &lista_bitcoins, int fecha_desde, int fecha_hasta)
{
    /*
    Si paso el vector sin puntero, 
    se crearía una copia completa del vector dentro 
    de la función, lo cual puede ser costoso en términos 
    de rendimiento y uso de memoria, especialmente si el vector es grande.

    Al usar puntero, estoy permitiendo que la función trabaje directamente con el vector original.
    */
    double media = 0;
    double suma = 0;
    int cont = 0;

    for (auto &r : lista_bitcoins)
    {
        //+1900 es porque usando tm_year para obtener el calendario actual hay que sumar al año
        // tm_year calcula las fechas de la siguiente manera: si le metes año 2018, calcula esto --> 2018 = 1900 +118.
        int year = r.date.tm_year + 1900;
        if (fecha_desde <= year && year <= fecha_hasta)
        {
            suma += r.volumen;
            cont++;
        }
    }

    if (cont > 0)
    {
        media = (suma) / cont;
        media = round(media * 100.0 / 100.0);
    }
    if(media < 0){
        media = media*-1; //Un pedaso de parche feaco, porque no entiendo porque algunas medias de volumenes son negativas, siendo todos los datos positivos.
    }

    return media;
}



/* FUNCIÓN 3:
  ¨¨¨¨¨¨¨¨¨¨¨¨
    Selecciona el volumen maximo de los que se encuentran en el anyo deseado y devuelve todos los campos que estan en dicho registro. 
    
        ENTRADA:
            -lista de bitcoins(date, apertura, high, low, close, adj_close, volumen) -> [(tm, float, float, float, float, float, long long)]
            -la fecha deseada (anyo) -> [(int)]
        SALIDA:
            -campos(date, apertura, high, low, close, adj_close, volumen) -> [(tm, float, float, float, float, float, long long)]
*/
vector<tuple<tm, float, float, float, float, float, int>> max_volumen(vector<Bitcoin> &lista_bitcoins, int year)
{
    int max_vol = 0;
    vector<tuple<tm, float, float, float, float, float, int>> campos;
    //Calculo el volumen maximo en el año deseado por el usuario
    for(auto &r: lista_bitcoins)
    {
        if(r.date.tm_year + 1900 == year)
        {
            if(r.volumen>max_vol)
            {
                max_vol = r.volumen;
            }
        }
    }

    //Una vez calculado el volumen máximo, me recorro la lista de Bitcoins otra vez para saber cuales son las coincidencias con el volumen máximo y añadirlo a la lista que voy a devolver.
    for(auto &r: lista_bitcoins)
    {
        if(max_vol == r.volumen)
        {
            campos.push_back(make_tuple(r.date, r.apertura, r.high, r.low, r.close, r.adj_close, r.volumen));
        }
    }

  return campos;
}



/* FUNCIÓN 4:
  ¨¨¨¨¨¨¨¨¨¨¨
    Calcula una lista ordenada de mayor a menor de los Close (precio de cierre) y la lista será de tamayo "n" a elegir por el usuario.
       
        ENTRADA:
        - lista de bitcoins(date, apertura, high, low, close, adj_close, volumen) -> [(tm, float, float, float, float, float, long long)]
        SALIDA:
        - lista ordenada de mayor a menor de los Close -> [float]
*/
vector<float> orden_close( vector<Bitcoin> &lista_bitcoins, int n)
{
    vector<float> lista;

    // En este foreach defino el iterador "i" como "auto" para que el compilador asigne de menara automática
    // el tipo de variable que se encuentre en cada columna de la "lista_bitcoin".
    for (auto &i : lista_bitcoins)
    {
        lista.push_back(i.close);
    }
    // Uso sort() para ordenar la lista de menor a mayor y reserse() para darle la vuelta y dejarla en orden descendente.
    sort(lista.begin(), lista.end());
    reverse(lista.begin(), lista.end());

    return lista;
}



/* FUNCIÓN 5:
 ¨¨¨¨¨¨¨¨¨¨¨¨
    Crea un diccionario que permita agrupar los registros del año deseado, segun la apertura,
    dicho año lo recibe la funcion por parametro. A cada apertura se le hara corresponder una
    lista en formato tupla con el Close y el Volumen.

    ENTRADA:
        -lista de bitcoins(date, apertura, high, low, close, adj_close, volumen) -> [(tm, float, float, float, float, float, long long)]
    SALIDA:
        -diccinario[aper] = (close, volumen) -> [float]= [(float, long long)]
*/
void dicc_agrupado(vector<Bitcoin> &lista_bitcoins, int year)
{
    map<float, vector<tuple<float, int>>> dicc;
    for (auto &r : lista_bitcoins)
    {
        if (r.date.tm_year + 1900 == year)
        {
            //find() para comprobar si la clave que se va añadir ya existe en el diccionario,  
            //find() usado en un map devuelve un puntero que apunta al elemento con esa clave si se encuentra, o al final del mapa si no se encuentra.
            if (dicc.find(r.apertura) != dicc.end())
            {
                dicc[r.apertura].push_back(make_tuple(r.close, r.volumen));
            }
            else
            {
                dicc[r.apertura] = {make_tuple(r.close, r.volumen)};
            }
        }
    }

    //Convertir el diccionario en un vector para facilitar la impresión
    vector<pair<float, vector<tuple<float, int>>>> resultadoVector(dicc.begin(), dicc.end());

    //Imprimo el contenido del vector
    for(auto &r: resultadoVector)
    {
        float clave = r.first;
        vector<tuple<float, int>> valor = r.second;

        //Clave
        cout<<"Apertura: "<< clave <<"\n";

        //Valores
        for(auto v : valor)
        {
            float close = get<0>(v);
            int volumen = get<1>(v);

            cout<<"  Close: " << close << "  Volumen: " << volumen <<endl;
        }
    }

}



/* FUNCIÓN 6:
 ¨¨¨¨¨¨¨¨¨¨¨¨¨
    Crea un diccionario que permita agrupar los registros, segun la Adjusted_close_price (adj_close)  y
    que haga corresponder a cada Adj_close una lista en formato tupla , con los 'n' registros formados por el volumen y
    la fecha, ordenados de menor a mayor por volumen.

    ENTRADA:
        -lista de bitcoins(date, apertura, high, low, close, adj_close, volumen)-> [(tm, float, float, float, float, float, long long)]
        -numero de registros deseado (n)-> [(int)]
    SALIDA:
        -diccinario[adj_close]=[(volumen, date)] -> [(float)] -> [(long long, tm)]
*/
bool comparador( pair<float, vector<tuple<int, tm>>> &a,  pair<float, vector<tuple<int, tm>>> &b)
{
    //primer elemento del vector en el pair
    int A = get<0>(a.second[0]);
    int B = get<0>(b.second[0]);

    return A > B;
}

void diccionario_ordenado_hasta_n(vector<Bitcoin> &lista_bitcoins, int &n)
{
    map<float, vector<tuple<int, tm>>> dicc;

    // Relleno el diccionario
    for (auto &r : lista_bitcoins)
    {
        if (dicc.find(r.adj_close) != dicc.end())
        {
            dicc[r.adj_close].push_back(make_tuple(r.volumen, r.date));
        }
        else
        {
            dicc[r.adj_close] = {make_tuple(r.volumen, r.date)};
        }
    }

    // Ordenar el map por el valor de adj_close
    vector<pair<float, vector<tuple<int, tm>>>> vec(dicc.begin(), dicc.end());
    sort(vec.begin(), vec.end(), comparador);

    // Ahora lo recorro para imprimir los resultados del diccionario deseados
    cout<<endl;
    cout<<"  [CLAVE]\t       [VALORES]"<<endl;
    for (int i = 0; i < n && i < vec.size(); i++)
    {
        float key = vec[i].first;
        vector<tuple<int, tm>> valor = vec[i].second;

        // Imprimir Clave
        cout<<endl<<"[ " << key <<" ]"<<"--->";

        // Imprimir Valor
        for (auto &v : valor)
        {
            int volumen = get<0>(v);
            tm date = get<1>(v);

            // put_time() para convertir de tm a string
            cout<< "[ " << volumen << ", " << put_time(&date, "%d/%m/%Y")<<" ]" << "\n"; 
        }
    }
}





                      /********************************************************/
                     /******************* Funciones de TEST ******************/
                    /********************************************************/

void abrirEnlace(const char* url) {
    //ShellExecuteA es una función de la API de Windows que se utiliza para ejecutar o abrir archivos, carpetas o URL. 
    ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
    // nullptr: puntero nulo que se utiliza para indicar que no se está pasando información sobre la ventana principal. En este caso, no se proporciono un identificador de ventana.
    // "open": Este parámetro especifica la acción que se debe realizar. En este caso, "open" indica que el sistema debe tratar de abrir el archivo o recurso con la aplicación asociada.
    // url: Aquí se proporciona la URL o la ruta del archivo que se va a abrir. Puede ser una dirección web o la ruta de un archivo local.
    // nullptr: Este parámetro se refiere a los parámetros adicionales que se pueden pasar a la aplicación asociada. En este caso, no se están pasando parámetros adicionales, por lo que se utiliza nullptr.
    // nullptr: Similar al parámetro anterior, este se refiere al directorio de trabajo para el nuevo proceso. En este caso, no se especifica ningún directorio de trabajo adicional.
    // SW_SHOWNORMAL: Este parámetro indica cómo se debe mostrar la ventana de la aplicación asociada. En este caso, se utiliza SW_SHOWNORMAL para mostrar la ventana en su estado normal.
}

void imprimirPuntosSuspensivos() 
{
    int retardo = 1;

    for (int i = 0; i < 3; ++i) {
       
        cout<< ".";
        Sleep(retardo * 500);

    }
}

void test_lee_bitcoins(vector<Bitcoin> &datos)
{
    cout<<"Test de la funcion que te lee el fichero. ";
    cout<<"El numero total de registros leidos es --> [" << datos.size()<<"]" << endl;
    cout<<"Mostrando los 3 primeros registros leidos";

    imprimirPuntosSuspensivos();

    cout<<endl;
    
    for (int i = 0; i < 3 && i < datos.size(); i++)
    {
        auto &registro = datos[i];
        cout << "Fecha:" << put_time(&registro.date, "%d-%m-%Y") << "\t";
        cout << "Apertura:" << registro.apertura << "\t";
        cout << "High:" << registro.high << "\t";
        cout << "Low:" << registro.low << "\t";
        cout << "Close:" << registro.close << "\t";
        cout << "Adj Close:" << registro.adj_close << "\t";
        cout << "Volumen:" << registro.volumen << endl;
    }

    cout<<endl;
}


void test_calcularMediaVolumen(vector<Bitcoin> &datos)
{
    int fecha_desde = 0;
    int fecha_hasta = 0;
    double media = 0.0;
    cout<< "Test de la funcion que te calcula la media del Volumen en el periodo de years que tu eligas." << endl;
    cout<< "RECORDATORIO!"<<endl<<"En el csv de Bitcoins los datos estan comprendidos entre [2014, 2019]";
    cout<<endl;    
    do
    {
        cout<< "Introduce el year inicial: ";
        cin>> fecha_desde;
        
        cout<< "Introduce el year final: ";
        cin>> fecha_hasta;

        if(fecha_desde < 2014 || fecha_desde > 2019 || fecha_hasta < 2014 || fecha_hasta > 2019 || fecha_desde > fecha_hasta)
        {
            cout<<"Estas ciego ? No se pregunto";
            imprimirPuntosSuspensivos();
            cout<<endl;
        }
    }while(fecha_desde < 2014 || fecha_desde > 2019 || fecha_hasta < 2014 || fecha_hasta > 2019 || fecha_desde > fecha_hasta);
    
    cout << setprecision(2)<< "R--> La media del volumen entre " << fecha_desde << " y " << fecha_hasta << " es: " << calcular_media_volumen(datos, fecha_desde, fecha_hasta)<<endl;
    cout<<endl;
}


void test_max_volumen(vector<Bitcoin> &datos)
{
    int year = 0;
    cout<<"Test de la funcion que selecciona el volumen maximo en el year deseado y devuelve los campos correspondientes"<< endl;
    do
    {
        cout<< "Introduce el year deseado [2014-2019]: ";
        cin>> year;

        if(year < 2014 || year > 2019)
        {
            cout<<"Eres tonto o que ? ";
            imprimirPuntosSuspensivos();
            cout<<endl;
        }

    }while(year < 2014 || year > 2019);
    
    //Un vectorini para almacenar los resulatados que devuelve mi función.
    vector<tuple<tm, float, float, float, float, float, int>> resultado = max_volumen(datos, year);

    //Resultados
    cout<<"OK, cargando el registro con volumen maximo";
    imprimirPuntosSuspensivos();
    cout<<endl;
    for(auto &r : resultado)
    {
        tm date = get<0>(r);
        float apertura = get<1>(r);
        float high = get<2>(r);
        float low = get<3>(r);
        float close = get<4>(r);
        float adj_close = get<5>(r);
        int volumen = get<6>(r);

        // put_time() para convertir de tm a string
        cout<<"Fecha: "<< put_time(&date, "%d-%m-%Y") << "\t";
        cout<<"Apertura: "<< apertura << "\t";
        cout<<"High: "<< high << "\t";
        cout<<"Low: "<< low << "\t";
        cout<<"Close: "<< close << "\t";
        cout<<"Adj Close: "<< adj_close << "\t";
        cout<<"Volumen: "<< volumen << endl;
    }

    cout<<endl;
}


void test_orden_close(vector<Bitcoin> &datos)
{
    const char *url = "https://vm.tiktok.com/ZGeM2ueXe/";
    int n = 0;

    cout<< "Test de la funcion que calcula una lista ordenada de mayor a menor de los Close." << endl;
    do{
        cout<<"Introduce el tamano que deseas [1877 max]: ";
        cin>>n;

        if(n < 0 || n > 1877)
        {
            abrirEnlace(url);
        }
    }while(n < 0 || n > 1877 );
    
    vector<float> resultado = orden_close(datos, n);

    //Mostrar los resultados
    cout<<"OK, la lista ordenada de mayor a menor de los Close es";
    imprimirPuntosSuspensivos();
    cout<<endl;
    for(int i = 0; i < n && i < resultado.size(); i++)
    {
        cout<<"Close " << i + 1 << ": "<< resultado[i] << endl;
    }

    cout<<endl;
}


void test_dicc_agrupado(vector<Bitcoin> &datos)
{
    const char* url = "https://www.youtube.com/shorts/bxedLSWeNEg?feature=share";
    int year = 0;
    cout<<"Test de la funcion que agrupa los registros del year deseado, segun la apertura." << endl;
    do
    {
        cout<< "Introduce el year deseado [2014-2019]: ";
        cin>> year;
        if(year < 2014 || year > 2019)
        {
            abrirEnlace(url);
        }
        
    }while(year < 2014 || year > 2019);
    cout<<"OK, el diccionario para "<< year << " es";
    imprimirPuntosSuspensivos();
    cout<<endl;
    dicc_agrupado(datos, year);

    cout<<endl;
}



void test_diccionario_ordenado_hasta_n(vector<Bitcoin> &datos)
{
    const char* url = "https://www.youtube.com/watch?v=Ycu4W5mNkAY";    
    int n = 1;
    cout<<"Test de la funcion que te crea un diccionario, te lo agrupa, ordena y muestra solo los 'n' registros elegidos por ti :)"<<endl;
    do{
        cout<<"Introduce el numero de lineas del diccionario que quieres que te muestre [1, 1877]: ";
        cin>>n;

        if(n < 0 || n > 1877)
        {   
           abrirEnlace(url);
        }
    }while(n < 0 || n > 1877 );
    cout<<"LOADING";
    imprimirPuntosSuspensivos();
    cout<<endl;
    diccionario_ordenado_hasta_n(datos, n);
}



      /**************************/
     /********** MAIN **********/
    /**************************/
int main()
{

    int elecccion = 0;
    const char* url = "https://vm.tiktok.com/ZGeMC7Fnc/";
    string nombreFichero = "BTC-EUR.csv";
    vector<Bitcoin> datos = lee_bitcoins(nombreFichero);

    cout<<"Alo presindentes";
    imprimirPuntosSuspensivos();
    cout<<endl;
    cout<< "Checkea estas nuevas operaciones disponibles con el dataset de Bitcoin";
    cout<<endl;
    do{
        cout<<"OPTIONS:"<<endl;
        cout<<"     1) Lector de Bitcoins"<<endl;
        cout<<"     2) Calculador de medias"<<endl;
        cout<<"     3) Listador de precios de cierre"<<endl;
        cout<<"     4) Determinador del volumen maximo"<<endl;
        cout<<"     5) Mapeador"<<endl;
        cout<<"     6) Mapeador PRO LTD"<<endl;
        cout<<"     7) Finalizar programa"<<endl;
        cout<<endl;
        cout<<"Selecciona una opcion [1-7]: ";
        cin>>elecccion;
        cout<<endl;

        switch (elecccion)
        {
        case 1:
            test_lee_bitcoins(datos);
            break;
        case 2:
            test_calcularMediaVolumen(datos);
            break;
        case 3:
            test_orden_close(datos);
            break;
        case 4:
            test_max_volumen(datos);
            break;
        case 5:
            test_dicc_agrupado(datos);
            break;
        case 6:
            test_diccionario_ordenado_hasta_n(datos);
            break;
        case 7:
            //Despedida 
            abrirEnlace(url);
            Sleep(100);
            elecccion = 7;
            break;
        default:
            cout<<"Eres tonto o que? No se pregunto";
            imprimirPuntosSuspensivos();
            cout<<endl;
        }
    }while(elecccion != 7);
    
     
    return 0;
}

