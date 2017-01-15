#pragma once

#include <stdlib.h>
#include <string>
#include <random>

#include "facecube.h"
#include "cubiecube.h"
#include "coordcube.h"
#include "search.h"

namespace Kociemba { namespace TwoPhase {

class Tools {
public:
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Check if the cube string s represents a solvable cube.
    // "":                    ( 0) Cube is solvable
    // "wrong facelet count": (-1) There is not exactly one facelet of each colour<br>
    // "wrong edge count":    (-2) Not all 12 edges exist exactly once
    // "edge flipped":        (-3) One edge has to be flipped
    // "wrong corner count":  (-4) Not all corners exist exactly once
    // "corner twisted":      (-5) One corner has to be twisted
    // "parity":              (-6) Two corners or two edges have to be exchanged
    // 
    static std::string verify(std::string const &facelets) {
        initStatics();
        
        int count[6] = { 0, 0, 0, 0, 0, 0 };
        for (int i = 0; i < facelets.length(); i++){
            if( i < 54 ){
                switch(facelets[i]){
                case 'U': ++count[U]; break;
                case 'R': ++count[R]; break;
                case 'F': ++count[F]; break;
                case 'D': ++count[D]; break;
                case 'L': ++count[L]; break;
                case 'B': ++count[B]; break;
                }
            }
        }
                            
        for (int i = 0; i < 6; i++){
            if (count[i] != 9){
                return "wrong facelet count";
            }
        }
        
        FaceCube fc(facelets);
        CubieCube cc;
        fc.toCubieCube(cc);
        return cc.verify();
    }

    /**
     * Generates a random cube.
     * @return A random cube in the string representation. Each cube of the cube space has the same probability.
     */
    template <typename RNG>
    static std::string randomCube( RNG &rng ) {
        initStatics();

        CubieCube cc;
        cc.setFlip(uniform(rng, CoordCube::N_FLIP));
        cc.setTwist(uniform(rng, CoordCube::N_TWIST));
        do {
            cc.setURFtoDLB(uniform(rng, CoordCube::N_URFtoDLB));
            cc.setURtoBR(uniform(rng, CoordCube::N_URtoBR));
        } 
        while ((cc.edgeParity() ^ cc.cornerParity()) != 0);
        FaceCube fc;
        cc.toFaceCube(fc);
        return fc.to_String();
    }
    
private:
    // uniform random int in range 0..n-1
    template<typename RNG> static size_t uniform( RNG &rng, size_t n ){
        return std::uniform_int_distribution<size_t>(0, n-1)(rng);
    }
    
};

}}
