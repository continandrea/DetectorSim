#ifndef Analysis_h
#define Analysis_h 1

#include "globals.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4VHitsCollection.hh"
#include "G4String.hh"

class PMTHit;
class AbsorberHit;
class HistoManager;
class Analysis {

  public:
    // Singleton Class
    static Analysis* GetInstance(){
      if(Analysis::singleton == NULL)
        Analysis::singleton = new Analysis();
      return Analysis::singleton;
    }
    
    // Class Utility Methods
    virtual ~Analysis();
    void Initilize();
    void CleanUp();

    // Accumulation Methods
    void PrepareNewEvent(const G4Event* anEvent);
    void PrepareNewRun(const G4Run* aRun);
    void EndOfEvent(const G4Event* anEvent);
    void EndOfRun(const G4Run* aRun);
    void SetNumOpticalPhotonsGenerated(G4int numPhotons);
  
  private:

    // Singleton Analysis
    Analysis();
    static Analysis *singleton;

    // Class interanls
    HistoManager* fHistoManager;/* Histogram manager */
    
    // Data Collections Varaibles
    G4double eDepEvent;         /* Energy deposition in the absoerber */
    G4int nOPAbsEvent;          /* Number optical photons created     */
    G4int nOPPMTEvent;          /* Number optical photons detected    */
};
#endif
