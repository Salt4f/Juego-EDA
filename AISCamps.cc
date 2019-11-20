#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME SCamps


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    vector<int> D = dwarves(me());
    for (int id : D) {
      Unit dwarf = unit(id);
      if (not pos_ok(dwarf.pos + Dir(Top))) command(id, Bottom);
      else {
        Cell topCell = cell(dwarf.pos + Dir(Top));
      
        if ((topCell.type == Cave or topCell.type == Rock) and topCell.owner != me()) {
          command(id, Dir(Top));
        }
        else command(id, Dir(random(1, DirSize - 1)));
      }
    }
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
