# upmem dimm power mean measurement

#### Context

An important feature of the umpem dimm SDK is the measurement of the power consumption of the
upmem dimm at runtime. This is called "power-profiling". The "power-profiling" gives 3 different
information about the power, the two extremums, min and max, and also the mean.

The implementation of those 3 estimators is done with limited ressources in practice,
and this has a non negligeable impact on the quality of the estimation. min and max
are quite straightforward in term of measurement, but the mean has a limited precision,
depending of different parameters that the user needs to understand. The objective of this
technical note is to formaly evaluates the quality of the "mean" estimation. We will show
that the confidence and precision of the mean estimator mostly depends on a single critical
parameter, called **Ti_min**, the shortest possible period of the **ina** (input current supply)
signal, which is the image of **p** the instantaneous power supply.

The principal limitation of our implementation is the sampling frequency of the
"power measurement subsystem", called **Fech**. **Fech = 10KHz** in the current
implementation. We will see that this frequency is is quite slow for our requirements.

#### Implementation of power measurement subsystem



```
  (10) power profiling subsystem
              __________________________________________________________________________________
             /                                                                             rank1|
     _______/________________________________________________________________________   _____   |
    |     								       rank0|  |uP15 |  |
    |     _____    _____    _____    _____      _____    _____    __ __    _____    |  |_____|  |
    |    |uP0  |  |uP1  |  |uP2  |  |uP3 |     |uP4 |   |uP5 |   |uP6 |   |uP7  |   |_____|     |
    |    |____ |  |____ |  |____ |  |____ |    |____ |  |____ |  |____ |  |_____|   |     |     |
    |       |_______|_________|_______|___________|_______|_________|_______|       |     |     |
    |                                 _____________             |   ________________|_____|     |
    |                                |  MCU        |            |  |                |           |
    |                                |    ______   |          __|__|__              |           |
    |                                |   |     |   |         |       |              |           |
    |                                |   | ADC |___|___ina___| DC/DC |              |          /
    |                                |   |_____|   |         |_______|              |_________/
    |                                |_____________|            |                   |
    |                                                           |                   |
    |___________________________________________________________|___________________|  
 
```

The measurement is done inside the "upmem dimm MCU" component, as it is the only part
to have such hardware capabilities in the "upmem dimm". To measure the dpu rank power
consumption, we actually measure the current supply only, and we takes the hypothesis
that the input voltage is constant. Thus, this is the current supply **ina** that provides
all the information about the rank power supply.

The instantaneous power signal is given by **(0)** below :

<!--- p(W) = ina(A) . vdd(V) --->
<img src="https://latex.codecogs.com/gif.latex?\mathbf{(0)}p(W)&space;=&space;ina(A)&space;.&space;vdd(V)" title="p(W) = ina(A) . vdd(V)" />

, with **p** in watt, **ina** in amp, **vdd** in volts.

The mean power consumption is given by **(1)** below :

<!--- \overline{P} = \frac{vdd}{n} \sum_{i=1}^{n} ina_i --->
<img src="https://latex.codecogs.com/gif.latex?\mathbf{(1)}\overline{P}&space;=&space;\frac{vdd}{n}&space;\sum_{i=1}^{n}&space;ina_i" title="\overline{P} = \frac{vdd}{n} \sum_{i=1}^{n} ina_i" />

, with **n** being the sample size (number of sample used by the estimator).

The problem here is that we must ensure that **(1)** gives good estimation of **P**, the **ina** mean.

#### **p** signal model

As we take vdd as constant, the estimation of **p** mean is equivalent to the estimation of **ina** mean.
**ina** signal may varry over time and we don't have any deterministic information about how it varry.
Howerver, laboratory observations clearly shows that **ina** could be approched with this simple
and generic stochastic model **(2)** below :

<!--- \mathbf{(2)};ina(t) = \sum_{i=0}^{i=N-1}(\prod_{T_i}^{} S_i ) * \delta (t - T_{i-1} ); S_i \sim \mathcal{N}(\mu,\,\sigma^{2})\ --->
<img src="https://latex.codecogs.com/svg.image?\mathbf{(2)};ina(t)&space;=&space;\sum_{i=0}^{i=N-1}(\prod_{T_i}^{}&space;S_i&space;)&space;*&space;\delta&space;(t&space;-&space;T_{i-1}&space;);&space;S_i&space;\sim&space;\mathcal{N}(\mu,\,\sigma^{2})\&space;" title="\mathbf{(2)};ina(t) = \sum_{i=0}^{i=N-1}(\prod_{T_i}^{} S_i ) * \delta (t - T_{i-1} ); S_i \sim \mathcal{N}(\mu,\,\sigma^{2})\ " />

In **(2)**, both **Ti** ans **Si** are random variables.
We cannot make no assumptions on about **Ti**, but hopefully, our experimantations show that **Si** could be modelized as
normal law, as it is the sum of a constant value plus a AWGN (additive white gaussian noise) signal :
Note that AWGN is standard and is commonly used as noise signal model, in electrical engineering and telecommunication literature.
<!--- \mathbf{(3)} S_i \sim \mathcal{N}(\mu,\,\sigma^{2})\ -->
<img src="https://latex.codecogs.com/gif.latex?\mathbf{(3)}&space;S_i&space;\sim&space;\mathcal{N}(\mu,\,\sigma^{2})\" title="\mathbf{(3)} S_i \sim \mathcal{N}(\mu,\,\sigma^{2})\" />

In the rest of the study, we will treat **Si** as **random variable** but we will not consider **Ti** as a random varivale that we want to modelize, but as a parameter we wants to **minimize**. More precisely, the objective of this technical note is to show that our estimator will be able to estimates the mean of the model **(2)** with a sufficient precision and confidence, with **Ti** parameter treated as a lower boundary condition.

The final parameter we actually wants to compute is **N** , the **sample size**.
**Ti**, **N** and **Fech** parameter, are linked by **(4)** below :

<!--- mathbf{(4)}Fech \geq \frac{N}{T_{i}}, \forall i--->
<img src="https://latex.codecogs.com/gif.latex?\mathbf{(4)}Fech&space;\geq&space;\frac{N}{T_{i}},&space;\forall&space;i" title="\mathbf{(3)}Fech \geq \frac{N}{T_{i}}, \forall i" /> 

Thus, **Ti** will be computed with **(4)**, taking a fixed sampling frequency **Fech**, that is an uncompressible maximal value that depend on the **power measurement subststem**.


#### On the estimation of the required sample size **N**

As explained before, the final parameter we want to compute is the minimal number of samples required to ensures that
our measure have a correct precision, that is called **N**.

The exstimator **(1)** of signal **(2)** mean is actually the same than **(5)** below,
which is the basic empirical mean formula.

<!--- \textbf{(5)}E[X]= \frac{1}{N} \sum_{i=1}^{N} X_i  -->
<a href="https://www.codecogs.com/eqnedit.php?latex=\textbf{(5)}E[X]=&space;\frac{1}{N}&space;\sum_{i=1}^{N}&space;X_i" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\textbf{(5)}E[X]=&space;\frac{1}{N}&space;\sum_{i=1}^{N}&space;X_i" title="\textbf{(4)}E[X]= \frac{1}{N} \sum_{i=1}^{N} X_i" /></a>

For esperance estimation of the complete signal **(2)**, we could formally show that considering the mean estimator of a sigle **Si** is equivalent.
In other words, for the parameters we wants to compute, we only need to consider one single random variable **Si**.
We will replace **(1)**, the empirical mean of **(2)**, by **(4)**, the empirical mean of **Xi**, with **Xi** being **Si** in our case.

In **(5)**, the critical parameter is **N**, the sample size.
Recall that we have limited sampling and computation capabilities in the MCU,
and so we need to verify that we reach certain objective, that is the estimation
precision and confidence.

Formally, in any estimation problem, we could say that we want **delta** below as small as possible in the formula below.

<a href="https://www.codecogs.com/eqnedit.php?latex=\textbf{(6)}&space;\overline{X}&space;-&space;\delta&space;\leq&space;\mu&space;\leq&space;\overline{X}&space;&plus;&space;\delta" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\textbf{(6)}&space;\overline{X}&space;-&space;\delta&space;\leq&space;\mu&space;\leq&space;\overline{X}&space;&plus;&space;\delta" title="\textbf{(6)} \overline{X} - \delta \leq \mu \leq \overline{X} + \delta" /></a>

To evaluates this, we use the **single sided confidence interval** theory, with **Xi** in **(5)**,
folowing normal law.
With **single sided confidence interval**, we can computes the statistical intervals for which
the esperance estimator result is the most likely to appear, given the parameter **phi**, that is
the probability that the estilator result doesn't fall out of  this interval (formally, **phi** is defined
as the risk of rejecting a true hypothesis). In **single sided confidence interval**, we admit that **sigma**,
the STD (standard deviation) of **X** is known. Thus, in practice, the choice of **sigma** is taken as
the maximal measured **ina** STD value, with some majoration (the STD is measured with the 6sigma method in lab,
on the real hw).


<!--- \mathbf{(7)} \mathbf{\sigma} = (1/6 * max(\6\sigma_{\mathbf{ina}})) -->
<a href="https://www.codecogs.com/eqnedit.php?latex=\mathbf{(7)}&space;\mathbf{\sigma}&space;=&space;(1/6&space;*&space;max(\6sigma_{\mathbf{ina}}))" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\mathbf{(7)}&space;\mathbf{\sigma}&space;=&space;(1/6&space;*&space;max(\6sigma_{\mathbf{ina}}))" title="\mathbf{(7)} \mathbf{\sigma} = (1/6 * max(\6\sigma_{\mathbf{ina}}))" /></a>

For a normal distribution, the confidence interval folows the formula below, with **k** defined as the **k quantile** of the normal law.
<!--- \mathbf{(8)} \mu - k\frac{\sigma}{^{\sqrt{n}}} \leq {\overline{X}} \leq \mu + k\frac{\sigma}{^{\sqrt{n}}} -->
<a href="https://www.codecogs.com/eqnedit.php?latex=\mathbf{(8)}&space;\mu&space;-&space;k\frac{\sigma}{^{\sqrt{n}}}&space;\leq&space;{\overline{X}}&space;\leq&space;\mu&space;&plus;&space;k\frac{\sigma}{^{\sqrt{n}}}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\mathbf{(8)}&space;\mu&space;-&space;k\frac{\sigma}{^{\sqrt{n}}}&space;\leq&space;{\overline{X}}&space;\leq&space;\mu&space;&plus;&space;k\frac{\sigma}{^{\sqrt{n}}}" title="\mathbf{(8)} \mu - k\frac{\sigma}{^{\sqrt{n}}} \leq {\overline{X}} \leq \mu + k\frac{\sigma}{^{\sqrt{n}}}" /></a>

We could simplify **(8)**, for one or more fixed well known **k** values, to :
<!--- \mathbf{(9)} N \geqslant  \lambda (\frac{\sigma}{\delta} )^{2},\delta=err *  \mu-->
<a href="https://www.codecogs.com/eqnedit.php?latex=\mathbf{(9)}&space;N&space;\geqslant&space;\lambda&space;(\frac{\sigma}{\delta}&space;)^{2},\delta=err&space;*&space;\mu" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\mathbf{(9)}&space;N&space;\geqslant&space;\lambda&space;(\frac{\sigma}{\delta}&space;)^{2},\delta=err&space;*&space;\mu" title="\mathbf{(9)} N \geqslant \lambda (\frac{\sigma}{\delta} )^{2},\delta=err * \mu" /></a>

, with **lambda** being the **k quantile critical value** of the normal law distribution function (that depends itself of **phi**, the degree of confidence).

#### Application

To computes the final sample size **N**, we will consider only two different values for **phi** (the degree of conficence) :

**phi** = 0.05 (95 % confidence) gives **lamba** = 1.96.

**phi** = 0.01 (99 % conficence) gives **lambda** = 2.576.


We will also computes **Ti_min**, the critical **ina** profile period, for a fixed
**Fech** of **10 KHz**, that is the minimal period mode (see **(1)**), for which our
estimation is correct it term of error and precision (see **(8)**).


confidence level 0.05 **(9)**                 
| relarive error |   N  | **Ti_min** us, Fech=10 KHz | 
|----- |:----:|:-----------------:|
|0.005 | 6521 | 652100           |
|0.01  | 1631 | 163100           |
|0.05  |   66 |   **6600**           | 
|0.1   |   17 |   1700           | 
|0.25  |    3 |    300           | 
|0.5   |    1 |    100           |
|0.8   |    1 |    100           |

confidence level 0.01 **(10)**
| relative error |   N  | **Ti_min** us, Fech=10 KHz  |
|----- |:----:|:-----------------:|
|0.005 | 11264| 1126400           |
|0.01  | 2816 |  281600           |
|0.05  |  113 |   11300           | 
|0.1   |   29 |   2900            | 
|0.25  |    5 |    500            | 
|0.5   |    2 |    200            |
|0.8   |    1 |    100            |


As interpretation, we could look at the case where Ti_min = **6600 us**,
which is one of the most releavant comared to the of the order of magnitude
that we observed in lab (see labs directory for **ina** signal captures).
Thus, this results of **6600 us** says that, for **p** mean estimation, our estimator has a 5 % error
with 95 % of confidence, providing that the critical condition that the **ina** input signal has a
**Ti** always less than **6600 us**.

#### [Apendix 0] Constants measured in lab session

**ina_sigma_amps** (**sigma of Si**) parameter of model **(2)** could be measured with good precision
with classical electronics measuring instruments like osciloscope, with the **6 sigma** graphical method.
That is sufficient for the expected precision.
Here **ina_mu_amps** parameter is the maximal possible mean of **Si** in **(2)**.
In practice **ina_mu_amps** is chosen as the upmem PCB power supply component, maximal current. 

| ina_sigma_amps |  ina_mu_amps | Ti us | 
|----- |:----:|:-----------------:|
|4.12 | 20 | TBC           |

#### [Apendix 1] Tables **(9)** and **(10)** application code :

```python
from tabulate import tabulate

fechMaxHz = 10e3
nLawlambdaNormalized = {0.05: 1.96, 0.01: 2.576}
relativeMarginErrPrct = [0.005, 0.01, 0.05, 0.1, 0.25, 0.5, 0.8]
inaStdAmps = 4.12
inaMuAmps = 20

for l in nLawlambdaNormalized:
    res = []
    for err in relativeMarginErrPrct:
        nHat = ((nLawlambdaNormalized[l]**2 * inaStdAmps**2) /
                (err**2 * inaMuAmps**2))
        res.append([])
        line = res[-1]
        line.append(err)
        line.append(int(1 + nHat))
        line.append(int(1e6 * line[-1] / fechMaxHz))
    print('confidence level', l)
    print(tabulate(res))
```
