#include <iostream>
#include <map>
#include <cmath>
#include <windows.h>
#include <valarray>
#include <conio.h>
#include <vector>
#undef min
#undef max
using namespace std;

double Trapezoid(double (*func)(double), double bot, double top, unsigned n) {
    double S_n = (func(top) + func(bot)) / 2, h = (top - bot) / n;
    for (unsigned i = 1; i < n; ++i) {
        S_n += func(bot + h * i);
    }
    S_n *= h;
    return S_n;
}

map<double, double> Euler(map<double, double> data, double accuracy, double (*func)(double, double)) {
    double x_i = data.rbegin()->first, y_i = data.rbegin()->second, step = accuracy;
    data[x_i + step] = y_i + step * func(x_i, y_i);
    return data;
    //O(h)
}

map<double, double> RungeKutt4(map<double, double> data, double accuracy, double (*func) (double, double)) {
    double x_i = data.rbegin()->first, y_i = data.rbegin()->second, step = sqrt(sqrt(accuracy));
    double K1, K2, K3, K4;
    K1 = step * func(x_i, y_i);
    K2 = step * func(x_i + step / 2, y_i + K1 / 2);
    K3 = step * func(x_i + step / 2, y_i + K2 / 2);
    K4 = step * func(x_i + step, y_i + K3);
    data[x_i + step] = y_i + (K1 + 2 * K2 + 2 * K3 + K4) / 6;
    return data;
    //O(h^4)
}

map<double, double> Adams(map<double, double> data, double accuracy, double (*func) (double, double)) {
    double x_i = data.rbegin()->first, y_i = data.rbegin()->second, step = sqrt(accuracy);
    map<double, double> :: iterator runner  = data.end();
    runner--;   runner--;
    data[x_i + step] = y_i + step * (3 / 2 * func(x_i, y_i) - 1 / 2 * func(runner->first, runner->second));
    return data;
    //O(h^2)
}

double Derivative(double x, double y) {
    return cos(y) / (1.25 + x) - 0.1 * pow(y, 2);
}

// главная функция обработки сообщений
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:// если этого не сделать, то все ваши жалкие попытки закрыть окно будут проигнорированы
		PostQuitMessage(0);// отправляет приложению сообщение WM_QUIT. Принимает код ошибки, который заносится в wParam сообщения WM_QUIT
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);//обрабатываем все остальные сообщения обработчиком "по умолчанию"
}
void  DrawWindow(valarray<double>* data, double(*f)(valarray<double>*, double)) {
	//Получаем хендл приложения, потребуется при создании класса окна и самого окна.
	HINSTANCE histance = GetModuleHandleW(NULL);
	//Создаем класс окна.
	WNDCLASSEX wclass = { 0 };          //Обнуляем структуру с самого начала, так как заполнять будем не все поля.
	wclass.cbSize = sizeof(WNDCLASSEX);      //По размеру структуры Windows определит, какая версия API была использована.
	wclass.style = CS_HREDRAW | CS_VREDRAW;    //Говорим окну перерисовываться при изменении размеров окна.
	wclass.lpfnWndProc = WndProc;        //Указываем функцию обработки сообщений.
	wclass.hInstance = histance;        //Указываем хендл приложения.
	wclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);    //GetStockObject возвращает хендл на белую кисточку, для фона окна
	wclass.lpszClassName = L"MYCLASS";      //Имя данного класса, должно быть уникальным, иначе, если класс с таким именем уже зарегестрирован, то в регистрации будет отказано.
	//Регистрируем класс окна.
	RegisterClassEx(&wclass);
	//Создаем окно.
	HWND window = CreateWindowExW(
		0,
		L"MYCLASS",                //Имя класса.
		L"Title",                //Заголовок окна.
		WS_OVERLAPPEDWINDOW,          //Тип окна, влияет на отображение системного меню, кнопок в верхнем правом углу и т.п.
		50, 50,                  //Координаты окна.
		320, 240,                //Ширина окна.
		0,                    //Ссылка на родительское окно.
		0,                    //Хендл меню.
		histance,                //Хендл приложения, получаем его функцией GetModuleHandleW.
		0
	);
	//Показываем окно, если этого не сделать окно не будет отображено.
	ShowWindow(window, SW_SHOW);
	//Обновляем окно.
	UpdateWindow(window);
	//Запускаем цикл обработки сообщений окна.
	MSG msg = { 0 };
	double x_max, x_min, diap_x;
	double y_max, y_min, diap_y;
	double y, x;
	y_min = data[1].min(); x_min = data[0].min();
	y_max = data[1].max(); x_max = data[0].max();
	diap_x = (x_max - x_min) / data[0].size();
	diap_y = (y_max - y_min) / data[1].size();                             //диапазон делений для оY 
	y_max += diap_y; y_min -= diap_y;
	HDC hDC = GetDC(window);                       //настройка okna для рисования 
	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));     //ручка для разметки 
	HPEN Pen1 = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));        //ручка для графика 
	HPEN Pen2 = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));        //ручка для точек
	//через прямоугольник rect описывается okno 
	RECT rect;
	GetClientRect(window, &rect);
	int width = 0;
	int height = 0;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);  //Преобразуем виртуальную клавишу в ASCII-код и посылаем сообщение WM_CHAR (тут не нужно.Необходимо, если надо работать с текстом, вводимым с клавиатуры)
		DispatchMessage(&msg);  //Передаем сообщения для обработки в "главную функцию обработки сообщений"
		GetClientRect(window, &rect);
		if (width != rect.right - rect.left || height != rect.bottom - rect.top) {
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
			SelectObject(hDC, Pen);
			//оси координат 
			MoveToEx(hDC, 0, height / 2, NULL);
			LineTo(hDC, width, height / 2);
			MoveToEx(hDC, width / 2, height, NULL);
			LineTo(hDC, width / 2, 0);
			//стрелка oY 
			MoveToEx(hDC, width / 2, 0, NULL);
			LineTo(hDC, width / 2 + 10, 10);
			MoveToEx(hDC, width / 2 - 10, 10, NULL);
			LineTo(hDC, width / 2, 0);
			//стрелка оХ 
			MoveToEx(hDC, width - 10, height / 2 - 10, NULL);
			LineTo(hDC, width, height / 2);
			MoveToEx(hDC, width, height / 2, NULL);
			LineTo(hDC, width - 10, height / 2 + 10);
			//деления oX
			for (x = x_min; x <= x_max; x += diap_x) {
				MoveToEx(hDC, width * (x - x_min) / (x_max - x_min), height / 2 - height / 250 - 1, NULL);
				LineTo(hDC, width * (x - x_min) / (x_max - x_min), height / 2 + height / 250 + 1);
			}
			//деления oY
			for (y = y_min; y < y_max; y += diap_y) {
				MoveToEx(hDC, width / 2 - width / 250 - 1, height * (y_max - y) / (y_max - y_min), NULL);
				LineTo(hDC, width / 2 + width / 250 + 1, height * (y_max - y) / (y_max - y_min));
			}
			//вывод графика и точек, по которому он строится
			SelectObject(hDC, Pen1);
			for (unsigned i = 0; i < data[0].size()-1; ++i) {
				MoveToEx(hDC, width * (data[0][i] - x_min) / (x_max - x_min), height * (y_max - data[1][i]) / (y_max - y_min), NULL);
				LineTo(hDC, width * (data[0][i+1] - x_min) / (x_max - x_min), height * (y_max - data[1][i+1]) / (y_max - y_min));
			}
		}
	}
	_getch();
}

double Lagrange(valarray<double>* data, double param) {
	double tmp, LagPol = 0;
	unsigned n = data[0].size();
	for (unsigned i = 0; i < n; ++i) {
		tmp = data[1][i];
		for (unsigned j = 0; j < n; ++j) {
			if (i == j) continue;
			tmp *= (param - data[0][j]) / (data[0][i] - data[0][j]);
		}
		LagPol += tmp;
	}
	return LagPol;
}

void MapToValarray(const map<double, double>& data, valarray<double>& data1, valarray<double>& data2) {
	data1.resize(data.size());
	data2.resize(data.size());
	unsigned i = 0;
	for (const auto& el : data) {
		data1[i] = el.first;
		data2[i] = el.second;
		++i;
	}
}

int main()
{
    map<double, double> data, tmp_map;
	valarray<double> data1[2];
    double tmp, acc, end, x_0;
	unsigned choice;
    cout << "Enter x_0 and y_0 : ";
    cin >> x_0;
    cin >> data[x_0];
    cout << "Enter your accuracy: ";
    cin >> acc;
    cout << "Enter end of segment : ";
    cin >> end;
	cout << "Сhoose a method:"<<endl;
	cout << "1) Calculate by Euler" << endl;
	cout << "2) Calculate by Runge - Kutt" << endl;
	cout << "3) Calculate by Adams" << endl;
	cin >> choice;
	if (!choice) {
		cout << "wrong number of choice!" << endl;
		return -1;
	}
	else if (choice == 1) {
		tmp_map = Euler(data, acc, Derivative);
		tmp = x_0;
		while (tmp <= end - acc) {
			tmp_map = Euler(tmp_map, acc, Derivative);
			tmp += acc;
		}
		for (const auto& el : tmp_map) {
			cout << el.first << "\t" << el.second << endl;
		}
		MapToValarray(tmp_map, data1[0], data1[1]);
		DrawWindow(data1, Lagrange);
	}
	else if (choice == 2) {
		cout << "Calculations by Runge-Kutt : " << endl;
		double step = sqrt(sqrt(acc));
		tmp_map = RungeKutt4(data, acc, Derivative);
		tmp = x_0;
		while (tmp <= end - 2 * step) {
			tmp_map = RungeKutt4(tmp_map, acc, Derivative);
			tmp += step;
		}
		for (const auto& el : tmp_map) {
			cout << el.first << "\t" << el.second << endl;
			unsigned size = data1[0].size();
			data1[0].resize(size + 1);
			data1[0][size] = el.first;
		}
		MapToValarray(tmp_map, data1[0], data1[1]);
		DrawWindow(data1, Lagrange);
	}
	else if (choice == 3) {
		cout << "Calculations by Adams : " << endl;
		tmp_map = RungeKutt4(data, acc, Derivative);
		tmp = x_0;
		double step = sqrt(acc), top = tmp_map.rbegin()->first;
		while (top <= end - step) {
			tmp_map = Adams(tmp_map, acc, Derivative);
			top = tmp_map.rbegin()->first;
		}
		for (const auto& el : tmp_map) {
			cout << el.first << "\t" << el.second << endl;
		}
		MapToValarray(tmp_map, data1[0], data1[1]);
		DrawWindow(data1, Lagrange);
	}
}
