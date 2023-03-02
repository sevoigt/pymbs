using MathNet.Numerics.LinearAlgebra;


int main()
{
    // single pendulum model    
    pendulum model = new pendulum();
    
    double[] y0 = new double[2]{0.1, 0};
    double[] yout;

    var res = model.pendulum_der_state(0, y0, out yout);

    //Console.WriteLine(yout[0].ToString());
    //Console.WriteLine(yout[1].ToString());

    // visualisation sensors
    pendulumVisual modelVis = new pendulumVisual();

    var y = CreateVector.DenseOfArray<double>(yout);
    var r = CreateVector.Dense<double>(3, 0);
    var T = CreateMatrix.Dense<double>(3, 3, 0);

    res = modelVis.pendulum_visual(y, out r, out T);

    Console.WriteLine(y.ToString());
    Console.WriteLine(r.ToString());
    Console.WriteLine(T.ToString());

    return 0;

}

main();