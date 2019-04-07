#ifndef PIECECONTAINER_H
#define PIECECONTAINER_H
#include"enums.h"
#include"PFMatrix.h"
#include"Piece.h"
#include "Renderer.h"
#include<vector>
#include<utility>
#include"ActiveInputs.h"
#include"TileContainer.h"


class PieceContainer : public Renderer
{
public:
    PieceContainer(){};
    PieceContainer(TileContainer * _tilecont, const nes_ushort& _frameappearance);
    Piece tryMove(const ActiveInputs& _inputs);
    Piece tryRotate(const ActiveInputs& _inputs);
    Piece tryDrop(const ActiveInputs& _inputs, const nes_uchar& _gravity);
    void inputManager(const ActiveInputs& _inputs, const PFMatrix& pfmatrix, const nes_uchar& _gravity);
    void doMove(const bool& _collision);
    void doRotate(const bool& _collision);
    void doDrop();
    void lockpiece(const nes_uchar& _lockheight);
    const Piece& getPiece() const;
    void render(const nes_ushort& _framecounter, const nes_uchar& _level);
    void rendernextpiece(const nes_uchar& _level);
    bool dropped;
    Piece lastdroppedpiece;
    nes_uchar holddownpoints;
    void hidecurrentpiece(const nes_uchar& _hidecurrent);

private:

    nes_uchar hidecountercurrentpiece=0;

    std::vector<std::pair<nes_uchar, nes_uchar> > lastrenderedpos;
    std::size_t spawncount=0; //TODO check spawncount
    nes_uchar spawnpiececounter;
    void spawnPiece(const nes_uchar& _spawndelay);

    bool downinterrupted=false, hidenextpiece; //TODO does select carry after new games?
    nes_uchar das=0, downcounter=0, holddowncounter=0;
    Piece currentpiece, nextpiece;

    static nes_uchar spawn_table[7];
};
#endif // PIECE_H
