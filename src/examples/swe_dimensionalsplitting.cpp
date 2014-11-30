#include <cassert>
#include <cstdlib>
#include <string>
#include <iostream>

#include "blocks/SWE_WavePropagationBlock.hh"
#include "writer/VtkWriter.hh"
#include "scenarios/SWE_simple_scenarios.hh"
#include "tools/args.hh"
#include "tools/help.hh"
#include "tools/Logger.hh"
//#include "tools/ProgressBar.hh"

/**
 * Main program for the simulation of dimensionalsplitting.
 */
int main( int argc, char** argv ) {
  /**
   * Initialization.
   */
  // Parse command line parameters
  tools::Args args;
  #ifndef READXML
  args.addOption("grid-size-x", 'x', "Number of cells in x direction");
  args.addOption("grid-size-y", 'y', "Number of cells in y direction");
  args.addOption("output-basepath", 'o', "Output base file name");

  //add Options for Simulation Time and Boundary Condition arguments
  args.addOption("simulated-time", 't', "Simulation time");
  args.addOption("boundary-condition", 'b', "Boundary Condition");
  #endif

  tools::Args::Result ret = args.parse(argc, argv);

  switch (ret)
  {
  case tools::Args::Error:
	  return 1;
  case tools::Args::Help:
	  return 0;
  }

  //! number of grid cells in x- and y-direction.
  int l_nX, l_nY;

  //! l_baseName of the plots.
  std::string l_baseName;

  int l_simTime;
  BoundaryType l_boundaryCond;

  // read command line parameters
  #ifndef READXML
  l_nX = args.getArgument<int>("grid-size-x");
  l_nY = args.getArgument<int>("grid-size-y");
  l_baseName = args.getArgument<std::string>("output-basepath");

  //read simulation-time and boundary-condition arguments
  l_simTime= args.getArgument<int>("simulation-time");
  std::string boundaryCondString= args.getArgument<std::string>("boundary-condition");
  if(boundaryCondString.compare("WALL")){
	  l_boundaryCond=WALL;
  }else if(boundaryCondString.compare("INFLOW")){
	  l_boundaryCond=INFLOW;
  }else if(boundaryCondString.compare("CONNECT")){
	  l_boundaryCond=CONNECT;
  }else if(boundaryCondString.compare("PASSIVE")){
	  l_boundaryCond=PASSIVE;
  }else{//Outflow per default
	  l_boundaryCond=OUTFLOW;
  }
  #endif

  // create a simple artificial scenario
  //SWE_RadialDamBreakScenario l_scenario;
  //SWE_ArtificialTsunamiScenario l_scenario;

  //TsunamiScenario with NetCDF
  TsunamiScenario l_scenario;
  l_scenario.simTime=l_simTime;
  l_scenario.boundaryCond=l_boundaryCond;


  //! number of checkpoints for visualization (at each checkpoint in time, an output file is written).
  int l_numberOfCheckPoints = 100;

  //! size of a single cell in x- and y-direction
  float l_dX, l_dY;

  // compute the size of a single cell
  l_dX = (l_scenario.getBoundaryPos(BND_RIGHT) - l_scenario.getBoundaryPos(BND_LEFT) )/l_nX;
  l_dY = (l_scenario.getBoundaryPos(BND_TOP) - l_scenario.getBoundaryPos(BND_BOTTOM) )/l_nY;

  // create a single wave propagation block
  SWE_DimensionalSplitting l_wavePropgationBlock(l_nX,l_nY,l_dX,l_dY);


  //! origin of the simulation domain in x- and y-direction
  float l_originX, l_originY;

  // get the origin from the scenario
  l_originX = l_scenario.getBoundaryPos(BND_LEFT);
  l_originY = l_scenario.getBoundaryPos(BND_BOTTOM);


  // initialize the wave propagation block
  l_wavePropgationBlock.initScenario(l_originX, l_originY, l_scenario);


  //! time when the simulation ends.
  float l_endSimulation = l_scenario.endSimulation();


  //! checkpoints when output files are written.
  float* l_checkPoints = new float[l_numberOfCheckPoints+1];

  // compute the checkpoints in time
  for(int cp = 0; cp <= l_numberOfCheckPoints; cp++) {
     l_checkPoints[cp] = cp*(l_endSimulation/l_numberOfCheckPoints);
  }

  // Init fancy progressbar
  //tools::ProgressBar progressBar(l_endSimulation);

  // write the output at time zero
  tools::Logger::logger.printOutputTime((float) 0.);
  //progressBar.update(0.);

  std::string l_fileName = generateBaseFileName(l_baseName,0,0);
  //boundary size of the ghost layers
  io::BoundarySize l_boundarySize = {{1, 1, 1, 1}};

  // consturct a VtkWriter
  io::VtkWriter l_writer( l_fileName,
		  l_wavePropgationBlock.getBathymetry(),
		  l_boundarySize,
		  l_nX, l_nY,
		  l_dX, l_dY );

  // Write zero time step
  l_writer.writeTimeStep( l_wavePropgationBlock.getWaterHeight(),
                          l_wavePropgationBlock.getDischarge_hu(),
                          l_wavePropgationBlock.getDischarge_hv(),
                          (float) 0.);


  /**
   * Simulation.
   */
  // print the start message and reset the wall clock time
  //progressBar.clear();
  tools::Logger::logger.printStartMessage();
  tools::Logger::logger.initWallClockTime(time(NULL));

  //! simulation time.
  float l_t = 0.0;

  //progressBar.update(l_t);

  unsigned int l_iterations = 0;

  // loop over checkpoints
  for(int c=1; c<=l_numberOfCheckPoints; c++) {

    // do time steps until next checkpoint is reached
    while( l_t < l_checkPoints[c] ) {
      // set values in ghost cells:
      l_wavePropgationBlock.setGhostLayer();

      // reset the cpu clock
      tools::Logger::logger.resetClockToCurrentTime("Cpu");

      // compute numerical flux on each edge
      l_wavePropgationBlock.computeNumericalFluxes();


      //! maximum allowed time step width.
      float l_maxTimeStepWidth = l_wavePropgationBlock.getMaxTimestep();

      // update the cell values
      l_wavePropgationBlock.updateUnknowns(l_maxTimeStepWidth);

      // update the cpu time in the logger
      tools::Logger::logger.updateTime("Cpu");


      // update simulation time with time step width.
      l_t += l_maxTimeStepWidth;
      l_iterations++;

      // print the current simulation time
      //progressBar.clear();
      tools::Logger::logger.printSimulationTime(l_t);
      //progressBar.update(l_t);
    }

    // print current simulation time of the output
    //progressBar.clear();
    tools::Logger::logger.printOutputTime(l_t);
    //progressBar.update(l_t);


    // write output
    l_writer.writeTimeStep( l_wavePropgationBlock.getWaterHeight(),
                            l_wavePropgationBlock.getDischarge_hu(),
                            l_wavePropgationBlock.getDischarge_hv(),
                            l_t);

  }


 /**
   * Finalize.
   */
  // write the statistics message
  //progressBar.clear();
  tools::Logger::logger.printStatisticsMessage();

  // print the cpu time
  tools::Logger::logger.printTime("Cpu", "CPU time");

  // print the wall clock time (includes plotting)
  tools::Logger::logger.printWallClockTime(time(NULL));

  // printer iteration counter
  tools::Logger::logger.printIterationsDone(l_iterations);

  return 0;
}
