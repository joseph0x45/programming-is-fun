#include <ncurses.h>

int main() {
  initscr();
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_RED);
  addstr("Normal text\n");
  attrset(COLOR_PAIR(1));
  addstr("Colored text whee");
  attrset(A_NORMAL);
  addstr("Back to normal");
  refresh();
  getch();
  endwin();
  return 0;
}
