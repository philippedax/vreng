// .NAME vtkFitsReader - read structured points from FITS file.
// .SECTION Description
// vtkFitsReader is a source object that reads FITS data files
// .SECTION Caveats
// Uses CFITSIO v2.0 (http://heasarc.gsfc.nasa.gov/docs/software/fitsio)

#ifndef __vtkFitsReader_h
#define __vtkFitsReader_h

#include "vtkStructuredPointsSource.h"
extern "C" {
  #include "fitsio.h"
}

class VTK_EXPORT vtkFitsReader : public vtkStructuredPointsSource
{
  public:
  vtkFitsReader();

  static vtkFitsReader *New() {return new vtkFitsReader;};
  const char *GetClassName() {return "vtkFitsReader";}
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetFileName(char *name);
  char *GetFileName(){return filename;}

  char title[80];
  char xStr[80];
  char yStr[80];
  char zStr[80];

  protected:
  void Execute();
  char filename[256]; // static buffer for filename

  void ReadHeader();
  void printerror(int status); // from fitsio distribution
};

#endif 

