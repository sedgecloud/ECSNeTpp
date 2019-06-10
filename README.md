# ECSNeT++
ECSNeT++ is a simulation toolkit for simulating the execution of 
Distributed Stream Processing applications on Edge anc Cloud Computing environments. 
ECSNeT++ is implemented using the [OMNeT++](https://omnetpp.org/) and the [INET framework](https://inet.omnetpp.org/).

For more information please contact [gamarasinghe@student.unimelb.edu.au](mailto:gamarasinghe@student.unimelb.edu.au).

# Dependencies
* [INET Framework 3.6.0](https://inet.omnetpp.org/)
* [TinyXML2](http://www.grinninglizard.com/tinyxml2/)

# Building the project

After cloning the repository run ```make makefiles``` followed by ```make```.

# Running the example

1. Set up OMNeT++ as instructed in [installation guide](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf).
2. Set up the [INET framework](https://inet.omnetpp.org/Installation.html).
3. Import ECSNeT++ to the OMNeT++ IDE.
4. Browse to ```Project Properties -> Project References``` and add ```inet``` project as reference.
5. Build the project.
6. Run ```omnetpp.ini``` file as an OMNeT++ simulation.
7. Select ```ETL-Pi3B-1-Plan``` and run the simulation and observe the ```simulation/results``` directory for simulation measurements.

# Extending the project

## Host devices

```WirelessHost``` module or the ```StandardHost``` module of the INET framework can be extended to build either a IEEE 802.11 wireless enabled device or a Ethernet enabled host device respectively.
See ```src/host``` package for examples.

### Network Support
We have created a [LTE plugin](https://github.com/sedgecloud/ECSNeT-LTE-Plugin) for ECSNeT++ to add LTE User Plane connectivity to the networking model using the [SimuLTE simulation tool](http://simulte.com/index.html).
An example is available in the github project of the plugin.

Similarly, other network models can also be adopted in to ECSNeT++ by creating hosts that use the network model.

## Distributed Stream Processing application

```StreamingSource```, ```StreamingOperator```, ```StreamingSink``` module are represent each Source, Operator and Sink in the topology. ECSNeT++
expects an adjacency matrix of the application topology (See ```configs/etl_app_topology.txt```) and a placement plan
(See ```configs/1.xml```). 

An example placement plan is shown below. The XML schema for generating the placement plan is available [here](https://github.com/sedgecloud/ECSNeTpp/blob/master/src/configs/placement-plan-schema.xsd).

```xml
<?xml version="1.0" ?>
<devices>
  <device>
    <name>danio-2</name>
    <index-range>0..49</index-range>
    <tasks>
      <task>
        <name>source</name>
        <category>source</category>
        <type>ecsnetpp.stask.StreamingSource</type>
        <processingdelay>
          <measuredtime>33406899</measuredtime>
        </processingdelay>
        <msgsize>6880</msgsize>
        <sourceevdistribution>
          <name>FixedSourceEventRateDistribution</name>
          <type>ecsnetpp.model.source.eventrate.FixedSourceEventRateDistribution</type>
        </sourceevdistribution>			
      </task>
      <task>
        <name>parser</name>
        <category>operator</category>
        <type>ecsnetpp.stask.StreamingOperator</type>
        <selectivitydistribution>
          <name>FixedSelectivityDistribution</name>
          <type>ecsnetpp.model.operator.selectivity.FixedSelectivityDistribution</type>
          <values>
            <selectivityratio>2</selectivityratio>
          </values>
        </selectivitydistribution>
          .
          .
          .
      </task>
    </tasks>
  </device>
  <device>
    <name>stargazer3</name>
    <index-range>0</index-range>
    <tasks>
      .
      .
      .
    </tasks>
  </device>
</devices>
```

### Source Characteristics

##### Source Event Rate
The `ecsnetpp.model.source.eventrate.ISourceEventRateDistribution` interface should be extended to implement different source event rate distributions.
See `ecsnetpp.model.source.eventrate.FixedSourceEventRateDistribution` module for an example.

#### Source Message Size
The `ecsnetpp.model.source.msgsize.IMessageSizeDistribution` interface should be extended to implement different source message size distributions. 
See `ecsnetpp.model.source.msgsize.FixedMessageSizeDistribution` module for an example.

### Operator Characteristics

#### Operator Selectivity Ratio
The `ecsnetpp.model.operator.selectivity.IOperatorSelectivityDistribution` interface should be extended to implement different operator selectivity ratio distributions.
See `ecsnetpp.model.operator.selectivity.FixedSelectivityDistribution` module for an example.

#### Operator Productivity Ratio
The `ecsnetpp.model.operator.productivity.IOperatorProductivityDistribution` interface should be extended to implement different operator productivity ratio distributions.
See `ecsnetpp.model.operator.productivity.FixedProductivityDistribution` module for an example.

## CPU Scheduling

### Changing the behaviour of the CPU scheduler
We have implemented a Round Robin Scheduler for selecting the CPU core for processing a streaming event at any task. It is possible to implement other scheduling strategies by implementing the `ecsnetpp.cpu.scheduling.ICpuCoreScheduler` interface.
The scheduler can be set at the host using the `cpuCoreSchedulerType` configuration (See one of the host devices for an example use of `ecsnetpp.cpu.scheduling.RoundRobinCpuCoreScheduler` as the core scheduler).
