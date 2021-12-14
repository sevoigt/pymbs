#Note: You need to have JModelica installed for this to work
#      and you have to start this from the JModelica PythonConsole
#      or set the needed environment variables in some other way.

# Import the function for compilation of models and the FMUModel class
from pymodelica import compile_fmu
from pyfmi import FMUModel
import matplotlib.pyplot as plt

# Load model
vdp = FMUModel("Output/CraneCrab.fmu")
#vdp = FMUModel("../Bugs/Output/Bug017.fmu")

# Simulate
res = vdp.simulate(final_time=10)

# Get the results
x1 = res['q_0']
x2 = res['q_1']
t  = res['time']

plt.figure(1)
plt.plot(t,x1,t,x2)
plt.show()
