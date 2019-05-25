# ECSNeT++
ECSNeT++ is a simulation toolkit for simulating the execution of 
Distributed Stream Processing applications on Edge anc Cloud Computing environments. 
ECSNeT++ is implemented using the [OMNeT++](https://omnetpp.org/) and the [INET framework](https://inet.omnetpp.org/).

For more information please contact [gamarasinghe@student.unimelb.edu.au](mailto:gamarasinghe@student.unimelb.edu.au).

## Building the project

After cloning the repository run ```make makefiles``` followed by ```make```.

## Running the example

1. Set up OMNeT++ as instructed in [installation guide](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf).
2. Set up the [INET framework](https://inet.omnetpp.org/Installation.html).
3. Import ECSNeT++ to the OMNeT++ IDE.
4. Browse to ```Project Properties -> Project References``` and add ```inet``` project as reference.
5. Build the project.
6. Run ```omnetpp.ini``` file as an OMNeT++ simulation.
7. Select ```ETL-Pi3B-1-Plan``` and run the simulation and observe the ```simulation/results``` directory for simulation measurements.

## Extending the project

### Host devices

```WirelessHost``` module or the ```StandardHost``` module of the INET framework can be extended to build either a IEEE 802.11 wireless enabled device or a Ethernet enabled host device respectively.
See ```src/host``` package for examples.

### Distributed Stream Processing application

```StreamingSource```, ```StreamingOperator```, ```StreamingSink``` module are represent each Source, Operator and Sink in the topology. ECSNeT++
expects an adjacency matrix of the application topology (See ```configs\etl_app_topology.txt```) and a placement plan
(See ```configs\1.xml```). 

An example placement plan is shown below.

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
        <type>ecs.stask.StreamingSource</type>
        <processingdelay>
          <measuredtime>33406899</measuredtime>
        </processingdelay>
        <msgsize>6880</msgsize>
        <sourceevdistribution>
          <name>FixedSourceEventRateDistribution</name>
          <type>ecs.model.source.eventrate.FixedSourceEventRateDistribution</type>
        </sourceevdistribution>			
      </task>
      <task>
        <name>parser</name>
        <category>operator</category>
        <type>ecs.stask.StreamingOperator</type>
        <selectivitydistribution>
          <name>FixedSelectivityDistribution</name>
          <type>ecs.model.operator.selectivity.FixedSelectivityDistribution</type>
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

#### Source Characteristics

##### Source Event Rate
The `ecsnet_.model.source.eventrate.ISourceEventRateDistribution` interface should be extended to implement different source event rate distributions.
See `ecsnet_.model.source.eventrate.FixedSourceEventRateDistribution` module for an example.

#### Source Message Size
The `ecsnet_.model.source.msgsize.IMessageSizeDistribution` interface should be extended to implement different source message size distributions. 
See `ecsnet_.model.source.msgsize.FixedMessageSizeDistribution` module for an example.

#### Operator Characteristics

#### Operator Selectivity Ratio
The `ecsnet_.model.operator.selectivity.IOperatorSelectivityDistribution` interface should be extended to implement different operator selectivity ratio distributions.
See `ecsnet_.model.operator.selectivity.FixedSelectivityDistribution` module for an example.

#### Operator Productivity Ratio
The `ecsnet_.model.operator.productivity.IOperatorProductivityDistribution` interface should be extended to implement different operator productivity ratio distributions.
See `ecsnet_.model.operator.productivity.FixedProductivityDistribution` module for an example.
