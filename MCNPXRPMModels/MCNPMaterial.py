#!/usr/bin/env python

class Materials(object):
    isotopeMass = {'H':1.008,'6Li':6,'C':12.011,'O':15.999,'F':18.998,
                '6LiF':25,'PS':104.149,'PEN':242.2}
    compounds = {'PS':{'C':8,'H':8},'PEN':{'C':14,'H':10,'O':4},'6LiF':{'6Li':1,'F':1}}
    zaaid = {'H':1001,'6Li':3006,'C':6012,'O':8016,'F':9019}
    
    def CheckMassFraction(self,val):
        """
        Checks if the mass fraction is within a range
        """
        if val <= 0:
            raise ValueError('Mass Fraction Value {} must be greater than 0!'.format(val))
        elif val > 1:
            raise ValueError('Mass Fraction Value {} must be less than 1!'.format(val))
        
    
    def ComputeCompositePolymer(self,q,polymer):
        """
        Creates a Compound Composite Polymer Molecular Formula
        
        Returns a dictionary between the isotopes and the number present
        """
        compound = dict()
        compound.update(self.compounds['6LiF'])
        compound.update(self.compounds[polymer])
        for iso in self.compounds[polymer]:
            compound[iso] = compound[iso]*q
        return compound
    
    def ComputeMassFraction(self,compound):
        """
        Computes the mass fraction of the compound
        
        Returns a dictionary between the isotopes and the mass fraction
        """
        # Computing the total mass
        massTotal = 0
        for i in compound:
            massTotal += self.isotopeMass[i]*compound[i]
        # Computing the mass fraction
        massFraction = dict()
        for i in compound:
            massFraction[i] = self.isotopeMass[i]*compound[i]/massTotal
        return massFraction
    
    def CreateMaterialString(self,massFraction):
        """
        Creates a material string for a given mass fraction
        """
        formatString = '{:d}.70c -{:5.4f} '
        s = 'm{:d}    '
        matStrings = list()
        for i in massFraction:
            matStrings.append(formatString.format(self.zaaid[i],massFraction[i]))
        matStrings = [matStrings[i:i+3] for i in range(0,len(matStrings),3)]
        s += '      '.join(''.join(i)+'\n' for i in matStrings)
        s.rstrip('\n')
        return s
    
    def CreateMCNPXMaterial(self,compound,mtNum):
        """
        Creates a MCNPX material of a given compound
        
        Keywords:
            compound - a dictionary between an isotope and the number of atoms
            mtNum    - material number in the MCNPX problem deck
        """
        massFraction = self.ComputeMassFraction(compound)
        matString = self.CreateMaterialString(massFraction)
        return matString.format(mtNum)
   
    def GetPSLiFCompound(self,massFractionLiF):
        """
        Returns the mass fraction of PS LiF
        keywords:
            massFractionLiF - the mass fraction of LiF in the material
        """
        self.CheckMassFraction(massFractionLiF)
        # See pg. 35 of 3rd Lab Notebook for derivation
        q = 0.238234/massFractionLiF -0.238234
        compound = self.ComputeCompositePolymer(q,'PS')
        massFraction = self.ComputeMassFraction(compound)
        return massFraction

    def GetPSLiF(self,massFractionLiF,mtNum=3):
        """
        Returns a MCNPX string representing the mateterial
        """
        compound = self.GetPSLiFCompound(massFractionLiF)
        return self.CreateMCNPXMaterial(compound,mtNum)
   
    def GetLiMassFraction(self,massFractionLiF,polymer):
        """
        Returns the mass fraction of 6-Li in the specified materail
        """
        if polymer == 'PS':
            data = self.GetPSLiFCompound(massFractionLiF)
        elif polymer == 'PEN':
            data = self.GetPENLiFCompound(massFractionLiF)
        else:
           raise ValueError('Polymer {} is not in the material database'.format(polymer)) 
        return data['6Li']
    
    def GetPENLiFCompound(self,massFractionLiF):
        """
        Creates a dictionary between the isotopes and the mass fractions
        for a composite PEN LiF Compound
        Keywords:
            massFractionLiF - the mass fraction of LiF in the material
        """
        self.CheckMassFraction(massFractionLiF)
        # See pg. 35 of 3rd Lab notebook for derivation
        q = 0.10322/massFractionLiF - 0.10322
        compound = self.ComputeCompositePolymer(q,'PEN')
        massFraction = self.ComputeMassFraction(compound)
        return massFraction

    def GetPENLiF(self,massFractionLiF,mtNum=3):
        """
        Returns the MCNPX material for the composite PEN LiF Material
        
        Keywords:
            massFractionLiF - the mass fraction of LiF in the material
        """
        compound = self.GetPENLiFCompound(massFractionLiF)
        return self.CreateMCNPXMaterial(compound,mtNum)
        
if __name__ == '__main__':
    M = Materials()
    print M.GetPSLiF(0.1,3)
    print M.GetPENLiF(0.3,4)
