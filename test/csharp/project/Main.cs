// See https://aka.ms/new-console-template for more information
using MathNet.Numerics.LinearAlgebra;

int main()
{
    Matrix<double> a = CreateMatrix.Dense<double>(2, 2, 5);

    //Console.WriteLine(a[0,0]);
    //Console.WriteLine((2*a)[1,0]);

    pendulum model = new pendulum();
    
    double[] y0 = new double[2]{1, 0};
    double[] yout;

    var res = model.pendulum_der_state(0, y0, out yout);

    Console.WriteLine(yout[0].ToString());
    Console.WriteLine(yout[1].ToString());

    return 0;

}

main();