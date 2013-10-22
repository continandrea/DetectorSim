#include "G4ios.hh"
#include "globals.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4EllipticalTube.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4OpBoundaryProcess.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4GeometryManager.hh"
#include "G4SDManager.hh"

#include "G4SolidStore.hh"
#include "G4RegionStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"

#include "G4RunManager.hh"

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "Materials.hh"
#include "PhotonDetSD.hh"

#include "G4UserLimits.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

PhotonDetSD* DetectorConstruction::pmtSD = NULL;

DetectorConstruction::DetectorConstruction()
 : physiWorld(NULL)
{

  detectorMessenger = new DetectorMessenger(this);
  materials = NULL;

  surfaceRoughness = 1;
 

  pmtPolish = 1.;
  pmtReflectivity = 0.;
    
    scintX = 30*cm;
    scintY = 100*um;
    scintZ = 100*cm;

    pmtLength = 1*cm;
 

  UpdateGeometryParameters();
}

DetectorConstruction::~DetectorConstruction()
{
  if (detectorMessenger) delete detectorMessenger;
  if (materials)         delete materials;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  materials = Materials::GetInstance();

  return ConstructDetector();
}

G4VPhysicalVolume* DetectorConstruction::ConstructDetector()
{
  //--------------------------------------------------
  // World
  //--------------------------------------------------

  G4VSolid* solidWorld = new G4Box("World", worldSizeX, worldSizeY, worldSizeZ);
  logicWorld = new G4LogicalVolume(solidWorld,FindMaterial("G4_AIR"),"World");
  physiWorld = new G4PVPlacement(0,G4ThreeVector(), logicWorld, "World", 0, false, 0);

  //--------------------------------------------------
  // Scintillator
  //--------------------------------------------------
    G4VSolid* solidScintillator = new G4Box("Scintillator",scintX/2,scintY/2,scintZ/2);
  G4LogicalVolume* logicScintillator = new G4LogicalVolume(solidScintillator,FindMaterial("Polystyrene"),"Scintillator");
  new G4PVPlacement(0, G4ThreeVector(), logicScintillator, "Scintillator", logicWorld, false, 0);
    
    
    //--------------------------------------------------
    // PhotonDet (Sensitive Detector)
    //--------------------------------------------------
    
    // Physical Construction
    G4double zOrig = scintZ+pmtLength/2;
    G4VSolid* solidPhotonDet = new G4Box("PhotonDet",scintX,scintY,pmtLength/2);
    G4LogicalVolume*   logicPhotonDet = new G4LogicalVolume(solidPhotonDet, FindMaterial("G4_Pyrex_Glass"), "PhotonDet");
    new G4PVPlacement(0,G4ThreeVector(0.0,0.0,zOrig), logicPhotonDet, "PhotonDet", logicWorld, false, 0);
    
    // PhotonDet Surface Properties
    G4OpticalSurface* PhotonDetSurface = new G4OpticalSurface("PhotonDetSurface", glisur, ground,dielectric_metal,pmtPolish);
    G4MaterialPropertiesTable* PhotonDetSurfaceProperty =new G4MaterialPropertiesTable();
    
    G4double p_pmt[2] = {2.00*eV, 3.47*eV};
    G4double refl_pmt[2] = {pmtReflectivity,pmtReflectivity};
    G4double effi_pmt[2] = {1, 1};
    
    PhotonDetSurfaceProperty -> AddProperty("REFLECTIVITY",p_pmt,refl_pmt,2);
    PhotonDetSurfaceProperty -> AddProperty("EFFICIENCY",p_pmt,effi_pmt,2);
    
    PhotonDetSurface -> SetMaterialPropertiesTable(PhotonDetSurfaceProperty);
    new G4LogicalSkinSurface("PhotonDetSurface",logicPhotonDet,PhotonDetSurface);
    if (!pmtSD) {
        G4String pmtSDName = "/PhotonDet";
        pmtSD = new PhotonDetSD(pmtSDName);
        
        G4SDManager* SDman = G4SDManager::GetSDMpointer();
        SDman->AddNewDetector(pmtSD);
    }
    
    // Setting the detector to be sensitive
    logicPhotonDet->SetSensitiveDetector(pmtSD);
    
  return physiWorld;
}


void DetectorConstruction::UpdateGeometry()
{
  if (!physiWorld) return;

  // clean-up previous geometry
  G4GeometryManager::GetInstance()->OpenGeometry();

  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  G4LogicalSkinSurface::CleanSurfaceTable();
  G4LogicalBorderSurface::CleanSurfaceTable();

  //define new one
  UpdateGeometryParameters();
 
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructDetector());

  G4RunManager::GetRunManager()->GeometryHasBeenModified();
  G4RunManager::GetRunManager()->PhysicsHasBeenModified();

  G4RegionStore::GetInstance()->UpdateMaterialList(physiWorld);
}

void DetectorConstruction::UpdateGeometryParameters()
{


  worldSizeX = scintX + pmtLength + 1.*cm;
  worldSizeY = scintY+ pmtLength + 1.*cm;
  worldSizeZ = scintZ+ pmtLength + 1.*cm;

}

// Set the Surface Roughness between Cladding 1 and WLS fiber
// Pre: 0 < roughness <= 1
void DetectorConstruction::SetSurfaceRoughness(G4double roughness)
{
    surfaceRoughness = roughness;
}


// Set the Polish of the PhotonDet, polish of 1 is a perfect mirror surface
// Pre: 0 < polish <= 1
void DetectorConstruction::SetPhotonDetPolish(G4double polish)
{
  pmtPolish = polish;
}

// Set the Reflectivity of the PhotonDet, reflectivity of 1 is a perfect mirror
// Pre: 0 < reflectivity <= 1
void DetectorConstruction::SetPhotonDetReflectivity(G4double reflectivity)
{
  pmtReflectivity = reflectivity;
}


G4Material* DetectorConstruction::FindMaterial(G4String name) {
    G4Material* material = G4Material::GetMaterial(name,true);
    return material;
}