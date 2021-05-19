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

void Euler(map<double, double>& data, double step, double (*func)(double, double)) {
    double x_i = data.rbegin()->first;
	data[x_i + step] = data[x_i] + step * func(x_i + step / 2, data[x_i] + step * func(x_i, data[x_i]) / 2) - data[x_i];
    //O(h)
}

void RungeKutt4(map<double, double>& data, double acc, double step, double (*func) (double, double)) {
    double x_i = data.rbegin()->first, y_i = data.rbegin()->second;
    double K1, K2, K3, K4;
    K1 = step * func(x_i, y_i);
    K2 = step * func(x_i + step / 2, y_i + K1 / 2);
    K3 = step * func(x_i + step / 2, y_i + K2 / 2);
    K4 = step * func(x_i + step, y_i + K3);
    data[x_i + step] = y_i + (K1 + 2 * K2 + 2 * K3 + K4) / 6;
    //O(h^4)
}

void Adams(map<double, double>& data, double acc, double step, double (*func) (double, double)) {
    double x_i = data.crbegin()->first, tmp;
	data[x_i + step] = data[x_i] + step * (3 / 2 * func(x_i, data[x_i]) - 1 / 2 * func(x_i - step, data[x_i - step]));
    //O(h^2)
}

double Derivative(double x, double y) {
	return 1 + 0.8 * y * sin(x) - 2 * pow(y, 2);
}

// главная функция обработки сообщений
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:// если этого не сделать, то все ваши попытки закрыть окно будут проигнорированы
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
	diap_x = max(fabs(x_max), fabs(x_min));
	diap_y = max(fabs(y_max), fabs(y_min));                             //диапазон делений для оY 
	y_max = diap_y; y_min = - diap_y;
	x_max = diap_x;	x_min = - diap_x;
	HDC hDC = GetDC(window);                       //настройка okna для рисования 
	HPEN Pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));     //ручка для разметки 
	HPEN Pen1 = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));        //ручка для графика 
	HPEN Pen2 = CreatePen(PS_SOLID, 6, RGB(255, 0, 0));        //ручка для точек
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
			MoveToEx(hDC, 0, height * (y_max) / (y_max - y_min), NULL);
			LineTo(hDC, width, height * (y_max) / (y_max - y_min));
			MoveToEx(hDC, width * (- x_min) / (x_max - x_min), height, NULL);
			LineTo(hDC, width * (-x_min) / (x_max - x_min), 0);
			//стрелка oY 
			MoveToEx(hDC, width * (-x_min) / (x_max - x_min), 0, NULL);
			LineTo(hDC, width * (-x_min) / (x_max - x_min) + 10, 10);
			MoveToEx(hDC, width * (-x_min) / (x_max - x_min) - 10, 10, NULL);
			LineTo(hDC, width * (-x_min) / (x_max - x_min), 0);
			//стрелка оХ 
			MoveToEx(hDC, width - 10, height * (y_max) / (y_max - y_min) - 10, NULL);
			LineTo(hDC, width, height * (y_max) / (y_max - y_min));
			MoveToEx(hDC, width, height * (y_max) / (y_max - y_min), NULL);
			LineTo(hDC, width - 10, height * (y_max) / (y_max - y_min) + 10);
			//деления oX
			for (x = x_min; x <= x_max; x += diap_x) {
				MoveToEx(hDC, width * (x - x_min) / (x_max - x_min), height * (y_max) / (y_max - y_min) - height / 250 - 1, NULL);
				LineTo(hDC, width * (x - x_min) / (x_max - x_min), height * (y_max) / (y_max - y_min) + height / 250 + 1);
			}
			//деления oY
			for (y = y_min; y < y_max; y += diap_y) {
				MoveToEx(hDC, width * (-x_min) / (x_max - x_min) - width / 250 - 1, height * (y_max - y) / (y_max - y_min), NULL);
				LineTo(hDC, width * (-x_min) / (x_max - x_min) + width / 250 + 1, height * (y_max - y) / (y_max - y_min));
			}
			//вывод графика и точек, по которому он строится
			SelectObject(hDC, Pen1);
			double step = (x_max - x_min) / 100;
			for (x = data[0].min(); x < data[0].max(); x += step) {
				MoveToEx(hDC, width * (x - x_min) / (x_max - x_min), height * (y_max - f(data, x)) / (y_max - y_min), NULL);
				LineTo(hDC, width * (x + step - x_min) / (x_max - x_min), height * (y_max - f(data, x + step)) / (y_max - y_min));
			}
			SelectObject(hDC, Pen2);
			for (unsigned i = 0; i < data[0].size(); ++i) {
				MoveToEx(hDC, width * (data[0][i] - x_min) / (x_max - x_min), height * (y_max - data[1][i]) / (y_max - y_min), NULL);
				LineTo(hDC, width * (data[0][i] - x_min) / (x_max - x_min), height * (y_max - data[1][i]) / (y_max - y_min));
			}
		}
	}
	_getch();
}

valarray<double> Gaus(vector <valarray<double>> a) {
	unsigned n = a.size();
	double coef;
	valarray<double> ans(n);
	for (unsigned i = 0; i < n; ++i) {
		unsigned j;
		for (j = i; j < n && a[j][i] == 0; ++j);
		if (j == n) {
			cout << "System has no solution or their quantity is infinity" << endl;
			exit(0);
		}
		else if (i != j)
			swap(a[j], a[i]);
		for (j = 0; j < i; ++j) {
			coef = a[j][i] / a[i][i];
			a[j] -= a[i] * coef;
		}
		for (j = i + 1; j < n; ++j) {
			coef = a[j][i] / a[i][i];
			a[j] -= a[i] * coef;
		}
	}
	for (unsigned i = 0; i < n; ++i) {
		ans[i] = (a[i][n] / a[i][i]);
	}
	return ans;
}

double Spline(valarray<double>* input, double param) {
	if (param < input[0][0]) throw invalid_argument("Can't find Splines for such argument!");
	vector<double> h;
	double ans;
	unsigned size = input[0].size(), i;
	vector<valarray<double>> output(size);
	valarray<double> m;
	for (unsigned i = 0; i < size - 1; ++i) {
		h.push_back(input[0][i + 1] - input[0][i]);
	}
	for (i = 0; i < size - 2; ++i) {
		output[i].resize(size + 1, 0);
		output[i][i + 1] = (h[i] + h[i + 1]) / 3;
		output[i][i + 2] = h[i + 1] / 6;
		output[i][size] = (input[1][i + 2] - input[1][i + 1]) / h[i + 1] - (input[1][i + 1] - input[1][i]) / h[i];
	}
	output[i].resize(size + 1, 0);
	output[i][0] = 1;
	output[i + 1].resize(size + 1, 0);
	output[i + 1][size - 1] = 1;
	m = Gaus(output);
	for (i = 1; i < size - 1 && input[0][i] < param; ++i);
	ans = ((m[i] * pow((param - input[0][i - 1]), 3) + m[i - 1] * pow(input[0][i] - param, 3)) / (h[i - 1] * 6) + (input[1][i] - m[i] * pow(h[i - 1], 2) / 6) * ((param - input[0][i - 1]) / h[i - 1]) + (input[1][i - 1] - m[i - 1] * pow(h[i - 1], 2) / 6) * (input[0][i] - param) / h[i - 1]);
	return ans;
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
    double tmp, acc, step, x_0;
	unsigned choice, order_acc;
    cout << "Enter x_0 and y_0 : ";
    cin >> x_0;
    cin >> data[x_0];
    cout << "Enter your accuracy: ";
    cin >> acc;
    cout << "Enter step: ";
    cin >> step;
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
		cout << "Calculations by Euler : " << endl;
		double last1, last2;
		order_acc = 1;
		tmp_map = data;
		do{
			Euler(data, step, Derivative);
			last1 = data.crbegin()->second;
			Euler(tmp_map, step/2.0, Derivative);
			Euler(tmp_map, step / 2.0, Derivative);
			last2 = tmp_map.crbegin()->second;
		} while (fabs(last1 - last2) > acc * (pow(2, order_acc)-1));
		for (const auto& el : tmp_map) {
			cout << el.first << "\t" << el.second << endl;
		}
		MapToValarray(tmp_map, data1[0], data1[1]);
		DrawWindow(data1, Spline);
	}
	else if (choice == 2) {
		cout << "Calculations by Runge-Kutt : " << endl;
		double last1, last2;
		order_acc = 4;
		tmp_map = data;
		double i = 0;
		do {
			RungeKutt4(data, acc, step, Derivative);
			last1 = data.crbegin()->second;
			RungeKutt4(tmp_map, acc, step / 2, Derivative);
			RungeKutt4(tmp_map, acc, step / 2, Derivative);
			last2 = tmp_map.crbegin()->second;
		} while (i += pow(step, 4), fabs(last1 - last2) + i < acc);//*(pow(4,order_acc)-1));
		for (const auto& el : tmp_map) {
			cout << el.first << "\t" << el.second << endl;
			unsigned size = data1[0].size();
			data1[0].resize(size + 1);
			data1[0][size] = el.first;
		}
		MapToValarray(tmp_map, data1[0], data1[1]);
		DrawWindow(data1, Spline);
	}
	else if (choice == 3) {
		cout << "Calculations by Adams : " << endl;
		tmp_map = data;
		RungeKutt4(data, acc, step, Derivative);
		RungeKutt4(tmp_map, acc, step, Derivative);
		RungeKutt4(tmp_map, acc, step, Derivative);
		double last1, last2;
		order_acc = 2;
		do {
			Adams(data, acc, step, Derivative);
			last1 = data.crbegin()->second;
			Adams(tmp_map, acc, step / 2, Derivative);
			Adams(tmp_map, acc, step / 2, Derivative);
			last2 = tmp_map.crbegin()->second;
		} while (fabs(last1 - last2) > acc * (pow(2, order_acc) - 1));
		for (const auto& el : tmp_map) {
			cout << el.first << "\t" << el.second << endl;
		}
		MapToValarray(tmp_map, data1[0], data1[1]);
		DrawWindow(data1, Spline);
	}
}
