#include <iostream>
#include <map>
#include <cmath>
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

map<double, double> RungeKutt4(map<double, double> data, double step, double (*func) (double, double)) {
    double x_i = data.rbegin()->first, y_i = data.rbegin()->second;// step = sqrt(sqrt(aссuracy));
    double K1, K2, K3, K4;
    K1 = step * func(x_i, y_i);
    K2 = step * func(x_i + step / 2, y_i + K1 / 2);
    K3 = step * func(x_i + step / 2, y_i + K2 / 2);
    K4 = step * func(x_i + step, y_i + K3);
    data[x_i + step] = y_i + (K1 + 2 * K2 + 2 * K3 + K4) / 6;
    return data;
    //O(h^4)
}

map<double, double> Adams(map<double, double> data, double step, double (*func) (double, double)) {
    double x_i = data.rbegin()->first, y_i = data.rbegin()->second;
    map<double, double> :: iterator runner  = data.end();
    runner--;   runner--;
    data[x_i + step] = y_i + step * (3 / 2 * func(x_i, y_i) - 1 / 2 * func(runner->first, runner->second));
    return data;
    //O(h^2)
}

double Derivative(double x, double y) {/*
    return 1 + 0.2 * sin(x) - pow(y, 2);
}

double f(double x, double y) {*/
    return cos(2 * x + y) + 1.5 * (x - y);
}

int main()
{
    map<double, double> data, tmp_map;
    double tmp, step, end, x_0;
    cout << "Enter x_0 and y_0 : ";
    cin >> x_0;
    cin >> data[x_0];
    cout << "Enter your step: ";
    cin >> step;
    cout << "Enter end of segment : ";
    cin >> end;
    cout << "Calculations by Euler : " << endl;
    tmp_map = Euler(data, step, Derivative);
    tmp = x_0;
    while(tmp <= end - 2 * step) {
        tmp_map = Euler(tmp_map, step, Derivative);
        tmp += step;
    }
    for (const auto& el : tmp_map) {
        cout << el.first << "\t" << el.second << endl;
    }
    cout << "Calculations by Runge-Kutt : " << endl;
    tmp_map = RungeKutt4(data, step, Derivative);
    tmp = x_0;
    while (tmp <= end - 2 * step) {
        tmp_map = RungeKutt4(tmp_map, step, Derivative);
        tmp += step;
    }
    for (const auto& el : tmp_map) {
        cout << el.first << "\t" << el.second << endl;
    }
    cout << "Calculations by Adams : " << endl;
    tmp_map = RungeKutt4(data, step, Derivative);
    tmp = x_0;
    while (tmp <= end - 2 * step) {
        tmp_map = Adams(tmp_map, step, Derivative);
        tmp += step;
    }
    for (const auto& el : tmp_map) {
        cout << el.first << "\t" << el.second << endl;
    }
}
