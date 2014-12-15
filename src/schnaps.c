#include "schnaps.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
  Field f;
  f.model.cfl = 0.05;
  f.model.m = 1; // only one conservative variable
  f.model.NumFlux = TransNumFlux2d;
  f.model.BoundaryFlux = TransBoundaryFlux2d;
  f.model.InitData = TransInitData2d;
  f.model.ImposedData = TransImposedData2d;
  f.varindex = GenericVarindex;

  f.interp.interp_param[0] = 1; // _M
  f.interp.interp_param[1] = 3; // x direction degree
  f.interp.interp_param[2] = 3; // y direction degree
  f.interp.interp_param[3] = 0; // z direction degree
  f.interp.interp_param[4] = 4; // x direction refinement
  f.interp.interp_param[5] = 4; // y direction refinement
  f.interp.interp_param[6] = 1; // z direction refinement

  // Read the gmsh file
  ReadMacroMesh(&(f.macromesh), "disque.msh");
  //ReadMacroMesh(&(f.macromesh), "geo/cube.msh");
  // Try to detect a 2d mesh
  bool is2d = Detect2DMacroMesh(&(f.macromesh));
  assert(is2d);
  //PrintMacroMesh(&(f.macromesh));

  // Mesh preparation
  BuildConnectivity(&(f.macromesh));

  // AffineMapMacroMesh(&(f.macromesh));

  // Prepare the initial fields
  InitField(&f);
  f.is2d = true;

  // prudence...
  CheckMacroMesh(&(f.macromesh), f.interp.interp_param + 1);

  printf("cfl param =%f\n", f.hmin);

  // Apply the DG scheme time integration by RK2 scheme up to final
  // time tmax.
  double tmax = 1.0;
  RK2(&f, tmax);

  // Save the results and the error
  PlotField(0, false, &f, NULL, "dgvisu.msh");
  PlotField(0, true, &f, "Error", "dgerror.msh");

  double dd = L2error(&f);
 
  printf("erreur L2=%f\n", dd);
  return 0;
};
