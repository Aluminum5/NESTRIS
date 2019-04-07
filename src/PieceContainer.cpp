#include "PieceContainer.h"
#include<stddef.h>
#include"enums.h"
#include"random.h"
#include<cstdio>

PieceContainer::PieceContainer(TileContainer * _tilecont, const nes_ushort& _frameappearance)
:Renderer(_tilecont, _frameappearance),
downinterrupted(false),
das(0),
downcounter(0),
spawncount(0),
hidenextpiece(false)
{
    spawnPiece(0);
    spawnPiece(0);
    hidecounter=sleepcounter=0;
}
bool collision(const PFMatrix& _pfmatrix, const Piece& _piece) {
    bool collision = false;
    std::vector<std::pair<nes_uchar, nes_uchar> > piecepositions = _piece.getPos();
    for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i=0; i<piecepositions.size(); ++i) {
        std::size_t _xx=piecepositions[i].first;
        std::size_t _yy=piecepositions[i].second;
        if (!PFMatrix::inbounds(_xx,_yy)) {
            collision = true;
            break;
        }
        if (_pfmatrix(_xx,_yy)) {
            collision = true;
            break;
        }
    }
    return collision;
}

void PieceContainer::inputManager(const ActiveInputs& _inputs, const PFMatrix& pfmatrix, const nes_uchar& _gravity) {
    dropped=false;
    if (_inputs.getPress(glb::SELECT)) hidenextpiece=!hidenextpiece;
    if (sleepcounter>0) {
        --sleepcounter;
        return;
    }
    if (glb::lineclearframecounter>0||glb::updatingmatrix>0) return; //TODO 1 frame error? updating>1?
    ++downcounter;
    //MOVE
    Piece temppiece=currentpiece;
    if (_inputs.getPress(glb::DOWN)) downinterrupted=false;
    if (_inputs.getHold(glb::DOWN)) {
        if (_inputs.getPress(glb::RIGHT)||_inputs.getPress(glb::LEFT)||_inputs.getHold(glb::RIGHT)||_inputs.getHold(glb::LEFT)) downinterrupted=true;
    }
    else {
        holddowncounter=holddownpoints=0;
        if (_inputs.getPress(glb::RIGHT)) {
            das=0;
            ++temppiece.x;
        }
        else if (_inputs.getPress(glb::LEFT)) {
            das=0;
            --temppiece.x;
        }
        else if (_inputs.getHold(glb::RIGHT)) {
            ++das;
            if (das>=16) {
                ++temppiece.x;
                das=10;
            }
        }
        else if (_inputs.getHold(glb::LEFT)) {
            ++das;
            if (das>=16) {
                --temppiece.x;
                das=10;
            }
        }
    }
    if (collision(pfmatrix,temppiece)) das=16;
    else currentpiece=temppiece;
    //ROTglb::ATE
    temppiece=currentpiece;
    if (_inputs.getPress(glb::A)) {
        temppiece.rotation=(temppiece.rotation-1)%4;
    }
    else if (_inputs.getPress(glb::B)) {
        temppiece.rotation=(temppiece.rotation-1)%4;
    }
    if (!collision(pfmatrix,temppiece)) currentpiece=temppiece;

    //ifnot holding down or have been holding down
    //DROP
    bool alreadymoveddown=false;
    temppiece=currentpiece;
    if (_inputs.getHold(glb::DOWN)&&!downinterrupted) {
        ++holddowncounter;
        ++holddownpoints;
        if (holddowncounter>=3) {
            alreadymoveddown=true;
            ++temppiece.y;
            holddowncounter-=2;
            if (collision(pfmatrix,temppiece)) {
                dropped=true;
                lastdroppedpiece=currentpiece;
            }
            else currentpiece=temppiece;
        }
    }
    if (downcounter>=_gravity &&!alreadymoveddown) {
        ++temppiece.y;
        downcounter=0;
        if (collision(pfmatrix,temppiece)) {
            dropped=true;
            lastdroppedpiece=currentpiece;
        }
        else currentpiece=temppiece;
    }
    //printf("das=%d\n",das);
}

const Piece& PieceContainer::getPiece() const{
    return currentpiece;
}

void PieceContainer::rendernextpiece(const nes_uchar& _level) {
    for (std::size_t x=glb::nextpiecex; x<glb::nextpiecex+4;++x)
        for (std::size_t y=glb::nextpiecey; y<glb::nextpiecey+4;++y)
            tilecont->at(x,y)=tiletype();
    if (!hidenextpiece) {
        typedef std::tuple<nes_schar, nes_schar, std::size_t> triple;
        std::vector<triple> nextpiecepos=nextpiece.nextpiecePos();
        for (const auto& i:nextpiecepos) {
            tilecont->at(std::get<0>(i),std::get<1>(i))=tiletype(std::get<2>(i),0x0d, 0x30, 0x12, 0x00);
        }
    }
}

void PieceContainer::render(const nes_ushort& _framecounter, const nes_uchar& _level) { //TODO the first piece renders a little bit late
    if (hidecounter>0) {
        --hidecounter;
        return;
    }
    if (glb::lineclearframecounter>0) {
        return;
    }
    if (hidecountercurrentpiece>0) {
        --hidecountercurrentpiece;
    }
    else {
        std::vector<std::pair<nes_uchar, nes_uchar> > piecepositions = currentpiece.getPos();
        for (std::vector<std::pair<nes_uchar, nes_uchar> >::size_type i=0; i<piecepositions.size(); ++i) {
            std::size_t _xx=piecepositions[i].first;
            std::size_t _yy=piecepositions[i].second;
            if (PFMatrix::visible(_xx,_yy)) {
                tilecont->at(glb::playfieldx+_xx,glb::playfieldy+_yy-2)=tiletype(_level,currentpiece.color());
            }
        }
    }
    rendernextpiece(_level);
}


void PieceContainer::spawnPiece(const nes_uchar& _spawndelay) { //TODO check if it's right
    currentpiece=nextpiece;
    nes_uchar spawnID=spawn_table[nextpiece.piecetype]; //creates a piece next to nextpiece
    ++spawncount;
    nes_uchar index=random::prng()>>8;
    index+=spawncount;
    index&=7;
    nes_uchar newSpawnID;
    if (index!=7) {
        newSpawnID = spawn_table[index];
        if (newSpawnID == spawnID) {
            random::prng();
            index=random::prng()>>8;
            index&=7;
            index+=spawnID;
            index%=7;
            newSpawnID = spawn_table[index];
        }
    }
    else {
        random::prng();
        index=random::prng()>>8;
        index&=7;
        index+=spawnID;
        index%=7;
        newSpawnID = spawn_table[index];
    }
    spawnID = newSpawnID;
    nes_uchar realID=0;
    for (std::size_t i=0; i<7;++i) {
        if (spawn_table[i]==spawnID) {
            realID=i;
            break;
        }
    }
    nextpiece.piecetype=realID;
    downcounter=holddowncounter=0;
}
void PieceContainer::hidecurrentpiece(const nes_uchar& _hidecurrent) {
    hidecountercurrentpiece=_hidecurrent;
}
void PieceContainer::lockpiece(const nes_uchar& _lockheight) {
    //printf("Lockpiece\n");
    nes_uchar _spawndelay=10+((_lockheight+3)/5)*2; //TODO fidn true formula
    spawnPiece(_spawndelay);
    downinterrupted=true; //TODO where to put this
}

nes_uchar PieceContainer::spawn_table[7]={0x02,0x07,0x08,0x0A,0x0B,0x0E,0x12};
