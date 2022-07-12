//SAM HORROBIN

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <EEPROM.h>

#define GAME_START 0
#define DISPLAY_S 1
#define CHOOSING_NEXT_CHAR 2
#define WAITING_FOR_PRESS 3
#define WAITING_FOR_RELEASE 4
#define DEAD_STATE 5

#define PLAY_STORY 0
#define PLAY_HS 1
#define RESET_HS 2
#define PLAY_PRACTICE 3
#define CHANGE_N 4
#define CHANGE_M 5
#define CHANGE_T 6
#define STORY_DIFFICULTY 500

#define INACTIVE 999

#define GAME_INACTIVE 9999

#define VIOLET 0x5
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define WHITE 0x7

int menuState;
int numMenuStates = 7;
int wait;

int gameState;
int currentPos = 0;
int N = 0;
int maxN = 10;
int NtoGen = 10;
char M[] = {'L', 'R', 'U', 'D'};
char *S;

int currentButton;
int changedButtons = 0;
int oldButtons = 0;
int buttons = 0;

long randomNo;
char chosenChar;

unsigned long Now = 0;
int checkTime;

int practiceMode;
int storyMode;
int hsMode;
int storyCounter = 0;
int MCount = 4;

float T = 2.0;
float T2 = 2.0;
float D = 1.0;
unsigned long releaseTimer = 0;
unsigned long Tcountdown = 0;

int currentHS;

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.setBacklight(WHITE);
  S = (char *) malloc(10*sizeof(char));
  gameState = GAME_INACTIVE;
  menuState = PLAY_STORY;
  randomSeed(analogRead(A0));
  currentHS = EEPROM.read(0);  
}

void loop() {

  //checks which buttons have changed.
  changedButtons = (oldButtons & (~(buttons = lcd.readButtons())));
  oldButtons = buttons;

  //this if loop contains all the code relating to the menu, in the form of a switch statement. It runs if the menuState variable is not set to inactive.
  //if menuState is inactive, the game is being played and gameState is not inactive, and vice versa.
  //each case of the switch statement corresponds to a different page of the menu (state).
  if (menuState != INACTIVE) {
    
    switch (menuState) {

      //the page where playing story mode can be selected from.
      case PLAY_STORY:
      
        lcd.setCursor(0, 0);
        lcd.print("PLAY STORY MODE");
        
        if (buttons & BUTTON_SELECT) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_SELECT)) {
              //changes the state of the menu to display the page where you select the difficulty level and sets the gameplay variables
              //so the sections of code relating to story mode will run and sections relating to other modes won't.
              menuState = STORY_DIFFICULTY;
              storyMode = 1;
              practiceMode = 0;
              hsMode = 0;
              wait = 0;
              lcd.clear();
            }
          }
        }
        
        else if (buttons & BUTTON_LEFT) {
          left_menu_change();
        }
        
        else if (buttons & BUTTON_RIGHT) {
          right_menu_change();
        }
        
        break;

      //this page is reached after you select to play story mode, and is where the user chooses the difficulty level to play.
      //this is the only state that cannot be reached as part of the main menu. All the others can.
      case STORY_DIFFICULTY:

        lcd.setCursor(0, 0);
        lcd.print("L: EASY  R: HARD");
        lcd.setCursor(0, 1);
        lcd.print("   D: NORMAL   ");

        //depending on what the user chooses, D and T are set to the appropriate vales and we switch to playing the game as opposed to being in the menu.
        if (buttons & BUTTON_LEFT) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_LEFT)) {
              gameState = GAME_START;
              menuState = INACTIVE;
              D = 1.2;
              T = 3.0;
              T2 = 3.0;
              wait = 0;
              lcd.clear();
              lcd.setCursor(0, 0);
            }
          }
        }

        else if (buttons & BUTTON_RIGHT) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_RIGHT)) {
              gameState = GAME_START;
              menuState = INACTIVE;
              D = 0.9;
              T = 1.0;
              T2 = 1.0;
              wait = 0;
              lcd.clear();
              lcd.setCursor(0, 0);
            }
          }
        }
        
        else if (buttons & BUTTON_DOWN) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_DOWN)) {
              gameState = GAME_START;
              menuState = INACTIVE;
              D = 1.2;
              T = 2.0;
              T2 = 2.0;
              wait = 0;
              lcd.clear();
              lcd.setCursor(0, 0);
            }
          }
        }

        break;

      //the page where playing high score mode can be selected from.
      case PLAY_HS:

        lcd.setCursor(0, 0);
        lcd.print("PLAY HIGH SCORE");
        lcd.setCursor(0, 1);
        lcd.print("MODE");
        
        if (buttons & BUTTON_SELECT) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_SELECT)) {
              
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("CURRENT HS: ");
              lcd.setCursor(12, 0);
              lcd.print(currentHS);

              Now = millis();
              checkTime = 1;
              while (checkTime) {
                if (millis() - Now > 2000) {
                  lcd.clear();
                  checkTime = 0;
                }
              }
              
              gameState = GAME_START;
              menuState = INACTIVE;
              storyMode = 0;
              practiceMode = 0;
              hsMode = 1;
              wait = 0;
              lcd.clear();
            }
          }
        }
        
        else if (buttons & BUTTON_LEFT) {
          left_menu_change();
        }
        
        else if (buttons & BUTTON_RIGHT) {
          right_menu_change();
        }
        
        break;

      //on this page the user can reset the highscore stored in memory.
      case RESET_HS:

        lcd.setCursor(0, 0);
        lcd.print("RESET HIGH SCORE");

        if (buttons & BUTTON_SELECT) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_SELECT)) {
              EEPROM.write(0, 10);
              lcd.setCursor(12, 1);
              lcd.print("DONE");
              currentHS = 10;
              wait = 0;
            }
          }
        }

        else if (buttons & BUTTON_LEFT) {
          left_menu_change();
        }
        
        else if (buttons & BUTTON_RIGHT) {
          right_menu_change();
        }
        
        break;      

      //on this page the user can choose to play practice mode. If they havent changed the values in the following pages, default values are used.
      case PLAY_PRACTICE:
  
        lcd.setCursor(0, 0);
        lcd.print("PLAY PRACTICE");
        lcd.setCursor(0, 1);
        lcd.print("MODE");
        
        if (buttons & BUTTON_SELECT) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_SELECT)) {
              gameState = GAME_START;
              menuState = INACTIVE;
              storyMode = 0;
              practiceMode = 1;
              hsMode = 0;
              wait = 0;
              lcd.clear();
            }
          }
        }
        
        else if (buttons & BUTTON_LEFT) {
          left_menu_change();
        }
        
        else if (buttons & BUTTON_RIGHT) {
          right_menu_change();
        }
        
        break;

      //this page appears as part of the main menu and allows the user to change the length of the sequenece they have to remember and enter in practice mode.
      case CHANGE_N:
  
        lcd.setCursor(0, 0);
        lcd.print("LENGTH OF");
        lcd.setCursor(0, 1);
        lcd.print("SEQUENCE:");
        lcd.setCursor(14, 1);
        lcd.print(maxN);
  
        if (buttons & BUTTON_LEFT) {
          left_menu_change();
        }
        
        else if (buttons & BUTTON_RIGHT) {
          right_menu_change();
        }
  
        else if (buttons & BUTTON_UP) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_UP)) {
              if (maxN < 10) {
                maxN++;
                NtoGen++;
              }
              wait = 0;
              lcd.clear();
            }
          }
        }
  
        else if (buttons & BUTTON_DOWN) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_DOWN)) {
              if (maxN > 1) {
                maxN--;
                NtoGen--;
              }
              wait = 0;
              lcd.clear();
            }
          }
        }
        
        break;

      //this page appears as part of the main menu and allows the user to change the size of the set of characters they have to remember and enter in practice mode.
      case CHANGE_M:
  
        lcd.setCursor(0, 0);
        lcd.print("SYMBOLS USED:");
        for (int i = 0; i < MCount; i++) {
          lcd.setCursor(i, 1);
          lcd.print(M[i]);
        }
  
        if (buttons & BUTTON_LEFT) {
          left_menu_change();
        }
        
        else if (buttons & BUTTON_RIGHT) {
          right_menu_change();
        }
  
        else if (buttons & BUTTON_UP) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_UP)) {
              if (MCount < 4) {
                MCount++;
              }
              wait = 0;
              lcd.clear();
            }
          }
        }
  
        else if (buttons & BUTTON_DOWN) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_DOWN)) {
              if (MCount > 1) {
                MCount--;
              }
              wait = 0;
              lcd.clear();
            }
          }
        }
        
        break;

      //this page appears as part of the main menu and allows the user to change the time limit to input each character in practice mode.
      case CHANGE_T:

        lcd.setCursor(0, 0);
        lcd.print("INPUT TIME LIMIT");
        lcd.setCursor(13, 1);
        lcd.print(T);
  
        if (buttons & BUTTON_LEFT) {
          left_menu_change();
        }
        
        else if (buttons & BUTTON_RIGHT) {
          right_menu_change();
        }
  
        else if (buttons & BUTTON_UP) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_UP)) {
              if (T < 2.8) {
                T=T+0.5;
                T2=T2+0.5;
              }
              wait = 0;
              lcd.clear();
            }
          }
        }
  
        else if (buttons & BUTTON_DOWN) {
          wait = 1;
          while (wait) {
            buttons = lcd.readButtons();
            if (!(buttons & BUTTON_DOWN)) {
              if (T > 0.6) {
                T=T-0.5;
                T2=T2-0.5;
              }
              wait = 0;
              lcd.clear();
            }
          }
        }
        
        break;    
    }
  }

  //this if loop contains all the code relating to playing the game itself, in the form of a switch statement. It runs if the gameState variable is not set to inactive.
  //if gameState is inactive, the menu is displayed and menuState is not inactive, and vice versa.
  //each case of the switch statement corresponds to a stage in the process of playing the game (state).
  if (gameState != GAME_INACTIVE) {
    switch (gameState) {

      //this state is ran first and sets any variables to their necessary values dependent on the mode, then generates the first sequence to display to the user.
      case GAME_START:

        if (hsMode) {
          NtoGen = 10;
          MCount = 4;
          T = 2.0;
          T2 = 2.0;
          D = 1.0;         
        }
        
        else if (storyMode) {
          NtoGen = 4;
          N = 0;
          currentPos = 0;
          
          if (storyCounter == 0) {
            MCount = 2;
            maxN = 7;
          }
          else if (storyCounter == 1) {
            MCount = 3;
            D = D - 0.2;
          }
          else if (storyCounter == 2) {
            MCount = 4;
            maxN = 10;
            D = D - 0.2;
          }
        }
        
        for (int i = 0; i < NtoGen; i++) {        
          randomNo = random(0, MCount);
          Serial.println(randomNo);
          chosenChar = M[randomNo];
          Serial.println(chosenChar);
          S[i] = chosenChar;
          N++;
          Serial.println(N);
          
          for (int i = 0; i < N; i++) {
            Serial.print(S[i]);
          }
          
          Serial.println();
          Serial.println();
          
        }
  
        gameState = DISPLAY_S;
  
        break;

      //this state displays the sequence to the user and starts countdowns for the user to input the first value of the sequence.
      case DISPLAY_S:
      
        for (int i = 0; i < N; i++) {
          checkTime = 1;
          while (checkTime) {
            //this if waits for the delay D to be reached before displaying the next character.
            if (millis() - Now > (D*1000)) {
              lcd.clear();
              lcd.print(S[i]);
              Now = millis();
              checkTime = 0;
            }
          }
  
          checkTime = 1;
          while (checkTime) {
            if (millis() - Now > (D*1000)) {
              lcd.clear();
              Now = millis();
              checkTime = 0;
            }
          }
        }
        
        lcd.print("ENTER SEQUENCE");

        T2 = T;
        lcd.setCursor(13, 1);
        lcd.print(T2);
        
        releaseTimer = millis();

        Tcountdown = millis();
        
        gameState = WAITING_FOR_PRESS;
        
        break;

      //this state is reached after the user completes a round and the sequence needs to be extended by 1.
      case CHOOSING_NEXT_CHAR:
      
        Now = millis();
        checkTime = 1;
        while (checkTime) {
          if (millis() - Now > 2000) {
            lcd.clear();
            lcd.setBacklight(WHITE);
            checkTime = 0;
          }
        }

        //the array S which stores the sequence starts at length 10. Once this array is filled this if loop runs and increases the size of the array by another 10.
        //when this array is filled it increases by 10 again, and so on.
        if ((N != 0) & ((N % 10) == 0)) {
          S = (char *) realloc(S, (N+10)*sizeof(char));
        }
  
        randomNo = random(0, MCount);
        Serial.println(randomNo);
        chosenChar = M[randomNo];
        Serial.println(chosenChar);
        S[N] = chosenChar;
        N++;
        Serial.println(N);
        
        for (int i = 0; i < N; i++) {
          Serial.print(S[i]);
        }
        
        Serial.println();
        Serial.println();
        currentPos = 0;
  
        gameState = DISPLAY_S;
  
        break;

      //this state is reached after the sequence has been displayed and the arduino waits for the user's input.
      case WAITING_FOR_PRESS:

        //checks if timer T has expired and if it has ends the game.
        if ((millis() - releaseTimer) > (T*1000)) {
          
          if (hsMode) {
            hs_game_over();
          }

          else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.setBacklight(RED);
            lcd.print("GAME OVER");
            gameState = DEAD_STATE;
          }
          
          break;
        }

        //checks if 0.5 seconds has passed and if it has it updates the value of T displayed to user
        if (millis() - Tcountdown > 500) {
          T2=T2-0.5;
          lcd.setCursor(13, 1);
          lcd.print(T2);
          Tcountdown = millis();
        }
        
        if (S[currentPos] == 'L') {
          currentButton = BUTTON_LEFT;
          //Serial.println(currentButton);
          //Serial.println("LEFT");
        }
        else if (S[currentPos] == 'R') {
          currentButton = BUTTON_RIGHT;
          //Serial.println(currentButton);
          //Serial.println("RIGHT");
        }
        else if (S[currentPos] == 'U') {
          currentButton = BUTTON_UP;
          //Serial.println(currentButton);
          //Serial.println("UP");
        }
        else {
          currentButton = BUTTON_DOWN;
          //Serial.println(currentButton);
          //Serial.println("DOWN");
        }

        //depending on what the user needs to input (currentButton) this bit of code checks whether they have pressed the correct button or the incorrect button, and proceeds accordingly.
        if (changedButtons & currentButton) {
          gameState = WAITING_FOR_RELEASE;
        }
  
        else if (changedButtons & (~(currentButton))) {

          if (hsMode) {
            hs_game_over();
          }

          else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.setBacklight(RED);
            lcd.print("GAME OVER");
            gameState = DEAD_STATE;
          }
        }
  
        break;

      //after they have pressed the correct button this state is entered. The code inside only runs once they release the button.
      case WAITING_FOR_RELEASE:
        
        if (!(buttons & currentButton)) {
          //runs if they are playing high score mode.
          if (hsMode) {
            //runs if they have completed the round
            if (currentPos == (N-1)) {
              //clears the characters the user entered that are shown on display if 10 characters displayed
              if ((currentPos != 0) & ((currentPos % 10) == 0)) {
                lcd.setCursor(0, 1);
                lcd.print("          ");
              }
              lcd.setCursor((currentPos%10), 1);
              lcd.print(S[currentPos]);
    
              Now = millis();
              checkTime = 1;
              while (checkTime) {
                if (millis() - Now > 500) {
                  checkTime = 0;
                }
              }
              
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.setBacklight(GREEN);
              lcd.print("NEXT ROUND");
              gameState = CHOOSING_NEXT_CHAR;
            }
            //runs if mid-round
            else {
              if ((currentPos != 0) & ((currentPos % 10) == 0)) {
                lcd.setCursor(0, 1);
                lcd.print("          ");
              }
              lcd.setCursor((currentPos%10), 1);
              lcd.print(S[currentPos]);
              currentPos++;
              T2 = T;
              lcd.setCursor(13, 1);
              lcd.print(T2);
              releaseTimer = millis();
              Tcountdown = millis();
              gameState = WAITING_FOR_PRESS;         
            }
          }
          
          
          else if (storyMode) {
            //runs if they have completed the current chapter
            if (currentPos == (maxN-1)) {
              lcd.setCursor(currentPos, 1);
              lcd.print(S[currentPos]);
              
              Now = millis();
              checkTime = 1;
              while (checkTime) {
                if (millis() - Now > 500) {
                  checkTime = 0;
                }
              }
              //this runs if they have completed story mode.
              if (storyCounter == 2) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.setBacklight(GREEN);
                lcd.print("COMPLETED");
                lcd.setCursor(0, 1);
                lcd.print("STORY MODE");
                gameState = DEAD_STATE;
              }
              else {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.setBacklight(GREEN);
                lcd.print("NEXT CHAPTER");
                gameState = GAME_START;
                storyCounter++;
                
                Now = millis();
                checkTime = 1;
                while (checkTime) {
                  if (millis() - Now > 2000) {
                    lcd.clear();
                    lcd.setBacklight(WHITE);
                    checkTime = 0;
                  }
                }
              }
            }
            //runs if they have completed round
            else if (currentPos == (N-1)) {
              lcd.setCursor(currentPos, 1);
              lcd.print(S[currentPos]);
    
              Now = millis();
              checkTime = 1;
              while (checkTime) {
                if (millis() - Now > 500) {
                  checkTime = 0;
                }
              }
              
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.setBacklight(GREEN);
              lcd.print("NEXT ROUND");
              gameState = CHOOSING_NEXT_CHAR;
            }
            //runs if they are mid-round
            else {
              lcd.setCursor(currentPos, 1);
              lcd.print(S[currentPos]);
              currentPos++;
              T2 = T;
              lcd.setCursor(13, 1);
              lcd.print(T2);
              releaseTimer = millis();
              Tcountdown = millis();
              gameState = WAITING_FOR_PRESS;         
            }
          }
          
  
          if (practiceMode) {
            //if they have completed practice mode
            if (currentPos == (maxN-1)) {
              lcd.setCursor(currentPos, 1);
              lcd.print(S[currentPos]);
              
              Now = millis();
              checkTime = 1;
              while (checkTime) {
                if (millis() - Now > 500) {
                  checkTime = 0;
                }
              }
              
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.setBacklight(GREEN);
              lcd.print("COMPLETED");
              lcd.setCursor(0, 1);
              lcd.print("PRACTICE MODE");
              gameState = DEAD_STATE;
            }
            //if they are mid-round
            else {
              lcd.setCursor(currentPos, 1);
              lcd.print(S[currentPos]);
              currentPos++;
              T2 = T;
              lcd.setCursor(13, 1);
              lcd.print(T2);
              releaseTimer = millis();
              Tcountdown = millis();
              gameState = WAITING_FOR_PRESS;         
            }
          }
        }
        
        break;

      //this state is reached once the game is finished, win or lose.
      case DEAD_STATE:

        //it resets the game to its original state (and displays the menu) so user can play again.
        //i.e. resets variables that have been changed and reverts menu and to original states.
         
        Now = millis();
        checkTime = 1;
        while (checkTime) {
          if (millis() - Now > 3000) {
            checkTime = 0;
          }
        }

        numMenuStates = 7;
        wait = 0;
        
        currentPos = 0;
        N = 0;
        maxN = 10;
        NtoGen = 10;
        
        changedButtons = 0;
        oldButtons = 0;
        buttons = 0;
        
        Now = 0;
        checkTime = 0;
        
        practiceMode = 0;
        storyMode = 0;
        hsMode = 0;
        storyCounter = 0;
        MCount = 4;
        
        T = 2.0;
        T2 = 2.0;
        D = 1.0;
        releaseTimer = 0;
        Tcountdown = 0;
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.setBacklight(WHITE);
        
        S = (char *) realloc(S, (10)*sizeof(char));
        gameState = GAME_INACTIVE;
        menuState = PLAY_STORY;
        randomSeed(analogRead(A0));
        currentHS = EEPROM.read(0);

        break;
              
    }
  }
}

//function called from menu states to 'scroll' once left in menu.
void left_menu_change() {
  wait = 1;
  while (wait) {
    buttons = lcd.readButtons();
    if (!(buttons & BUTTON_LEFT)) {
      Serial.println(menuState);
      if (menuState == 0) {
        menuState = (numMenuStates-1);
      }
      else {
        menuState = ((menuState-1)%numMenuStates);
      }
      Serial.println(menuState);
      Serial.println();
      wait = 0;
      lcd.clear();
    }
  }
}

//function called from menu states to 'scroll' once right in menu.
void right_menu_change() {
  wait = 1;
  while (wait) {
    buttons = lcd.readButtons();
    if (!(buttons & BUTTON_RIGHT)) {
      Serial.println(menuState);
      menuState = ((menuState+1)%numMenuStates);
      Serial.println(menuState);
      Serial.println();
      wait = 0;
      lcd.clear();
    }
  }
}

//function that is called when high score mode finishes.
void hs_game_over() {
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setBacklight(RED);
  lcd.print("GAME OVER");

  Now = millis();
  checkTime = 1;
  while (checkTime) {
    if (millis() - Now > 2000) {
      checkTime = 0;
    }
  }

  //checks to see if new high score has been set.
  if ((N-1) > currentHS) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.setBacklight(GREEN);
    lcd.print("NEW HIGH SCORE");

    Now = millis();
    checkTime = 1;
    while (checkTime) {
      if (millis() - Now > 2000) {
        checkTime = 0;
      }
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("OLD HS: ");
    lcd.setCursor(8, 0);
    lcd.print(currentHS);
    currentHS = (N-1);
    lcd.setCursor(0, 1);
    lcd.print("YOUR NEW HS: ");
    lcd.setCursor(13, 1);
    lcd.print(currentHS);

    byte toWrite = currentHS;
    EEPROM.write(0, toWrite);
    
    gameState = DEAD_STATE;
  }

  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NO NEW");
    lcd.setCursor(0, 1);
    lcd.print("HIGH SCORE");
    
    Now = millis();
    checkTime = 1;
    while (checkTime) {
      if (millis() - Now > 2000) {
        checkTime = 0;
      }
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HS: ");
    lcd.setCursor(8, 0);
    lcd.print(currentHS);
    lcd.setCursor(0, 1);

    //if they didn't input first sequence correctly
    if ((N-1) == 9) {
      lcd.print("YOUR SCORE: ");
      lcd.setCursor(12, 1);
      lcd.print("0");
    }
    
    else {
      lcd.print("YOUR SCORE: ");
      lcd.setCursor(12, 1);
      lcd.print((N-1));
    }

    gameState = DEAD_STATE;
  }
}
