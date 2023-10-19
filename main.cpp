#include <iostream>
#include <string>
#include <vector>

using namespace std;

// there are ten turns, which each has 2 "throws"
    // turn one gets all pins -> strike
    // turn two gets all pins -> spare
    // edge case: if scored a spare or strike on 10th turn get 1 extra for spare, 2 extra for strike, non-repeating

// input = a string of chars:
    // 1-9 = numerical amount of pins hit, for a throw
    // 'x' = strike, '/' = spare, '-' = miss

// scoring:
    // miss = 0, 1-9 = 1-9
    // Strikes and spares are different, they retroactively add the number of pins hit(not the scores themselves) from the next throws to their own
        // strikes: the score is 10 + a + b where a and b are the next throws
        // spares: same as strike, but only 1 extra 


int slidingWindowsScore(string input){
    string::iterator iter = input.begin();
    vector<int> throws;
    string::iterator peek1 = iter+1;
    string::iterator peek2 = iter+3;
    int inputLen = input.size();

    int iterHit = 0;
    int peek1Hit = 0;
    int peek2Hit = 0;
    int score = 0;
    for (int roll = 0; roll < 19; roll++)
    {
        iterHit = 0;
        peek1Hit = 0;
        peek2Hit = 0;
        if(*iter == '-' || isdigit(*iter)){             // 1-9 
            throws.push_back(*iter - 48 *isdigit(*iter));    // evaluates to zero if a miss
        }
        else if(*iter == '/' && roll% 2 == 1){          // / ?
            if(*peek1 == 'X'){                              // / X
                throws.push_back(20 - throws.back());
            }
            else if(isdigit(*peek1)){                       // / 1-9
                throws.push_back(10 - throws.back() + *peek1 - 48);

            }
        }
        else if (*iter == 'X' && roll % 2 == 0){        // X ? ?
            roll++;
            peek1 += 1;
            peek2 += 1;
            if(isdigit(*peek1)){                            // X 1-9 ?
                if(isdigit(*peek2)){                            // X 1-9 1-9
                    throws.push_back(10 + *peek1 - 48 + *peek2 - 48);
                }
                else if(*peek2 == '/'){                         // X 1-9 /
                    throws.push_back(20);
                }
            }
            else if(*peek1 == 'X'){                     // X X ?
                if(isdigit(*peek2)){                        // X X 1-9
                    throws.push_back(20 + *peek2 - 48);
                }
                else if(*peek2 == 'X'){                     // X X X
                    throws.push_back(30);
                }
            }
        }
        score += throws.back();
        cout << "headers: " << *iter <<" "<< *peek1 << " " << *peek2 << " score: " << throws.back() << endl;

        iter  = input.begin() + 3*(roll / 2) % inputLen;
        peek1 = input.begin() + 3*((roll/ 2) + 1) % inputLen;
        peek2 = input.begin() + 3*((roll/ 2) + 2) % inputLen;
    }
    
    return score;
}


int calculateTurnScore(vector<int>& specialThrows, vector<int>& throws, string::iterator& iter,string::iterator& iter_end, int turn){
    int roll_score = 0;
    int score = 0;
    for (int roll = 0; roll <= 1; roll++){          
        roll_score = 0;
        // if number -> save score, sum up score
        if (isdigit(*iter)){
            roll_score += *iter - 48;
            if(turn <= 9)
            {
                score += roll_score;
            }
        }
        // if strike or spare -> add turn(throw id) to filo structure
        else if (roll % 2 == 0 || turn > 9){ // Only first throws of frames allow strikes
            if (*iter == 'x')
            {
                if (turn > 9){
                    roll_score = 10;    // do not add strike to specialThrows, as it does not have the retroactive nature
                    iter ++;
                }
                else{
                    specialThrows.push_back(throws.size());
                    roll_score = -2;    
                    roll++;
                }
            }        
        }
        else if (roll % 2 == 1 || turn > 9){ // Only second throws of frames allow spares
            if (*iter == '/'){
                if (turn > 9){
                    roll_score = 10 - throws.back();    // do not add spare to specialThrows, as it does not have the retroactive nature
                }
                else{
                    
                    if (turn == 9){
                        iter--;         // if a spare is scored in the tenth round, its predecessor input is specified to be without whitespace in between
                    } 

                    specialThrows.push_back(throws.size()); // derived from from: (x - x_min) * (y_max - y_min + 1) + (y - y_min), where x = turn and y = roll
                    roll_score = -1;
                }
            }
        }
        else{
            cout<< "Input Error at turn " << turn +1<< endl; 
        }
        throws.push_back(roll_score);
        iter++;
        if(iter == iter_end){
            break;
        }
    }
    return score;
}

// takes input of form "5/ 5/ 5/ 5/ 5/ 5/ 5/ 5/ 5/ 5/5" note that spares on 10th frame dont incur whitespace, a strike would
int bowlingMethod(string input){
    vector<int> specialThrows;
    vector<int> throws;
    auto iter = input.begin(); 
    string::iterator iter_end = input.end(); 
    int turn = 0;
    int score = 0;
    int roll_score = 0;
    if(iter == iter_end){
        return -1;
    }
    // main loop
    for ( turn; turn <= 10; turn++)  
    {    
        score += calculateTurnScore(specialThrows,throws,iter,iter_end,turn);
        if(score == -1){
            return -1;
        }
        if(iter == iter_end)
        {break;}
        iter++;
    }

    // for all elements in specialThrows, we will retroactively now handle their scorings, by
    int element = 0;
    int count = specialThrows.size();
    int points[21]{0};  // purely a debugging structure, which counts a summed points per score, in the coming for loop the scores for strikes and spares are added, later the rest of the values are handled
    for (int i = 0; i < count; i++)
    {
        // handle strikes and spare scorings
        element = specialThrows.back();
        if(throws[element] == -1){  // spare
            throws[element] = 10 - throws[element-1];   // spares technically dont hit 10 pins, instead they hit 10- previous shot
            score += 10 + throws[element+1] - throws[element-1];    // remember to subtract previous score, to maintain correct scoring I.e '5/' != 15 points, but 10 points

            points[element] =  10 + throws[element+1];
            cout<< "Special score at " << element << ": " << 10 + throws[element+1] - throws[element-1] << endl;
        }
        else if (throws[element] == -2){  // strike
            throws[element] = 10;
            score += 10 + throws[element+1] + throws[element+2];

            points[element] = 10 +throws[element+1] + throws[element+2];
            cout<< "Special score at " << element << ": " << 10 + throws[element+1] + throws[element+2] << endl;
        }
        else{
            cout<< "Error at specialThrows for loop, element " << element<< " was " << throws[element] << endl;
            return -1;
        }
        specialThrows.pop_back();
        
    }
    int runningScore= 0;
    for (int i = 0; i < throws.size(); i++) // debugging for loop, which will print scores for each round
    {
        if(points[i] == 0){
            points[i] = throws[i];
        }
        if(throws[i] + throws[i-1] == 10 && turn > 0 && throws[i-1] != 0 && throws[i] != 0){
            runningScore -= throws[i-1];
        }
        runningScore += points[i];
        cout << "score at throw " << i << ": "<< runningScore << endl;
    }
    
    return score;
}

int main(){
    // take input

    string input = "";
    while (input != "quit")
    {
        cout << "Please input throws:";
        getline(cin, input);
        //int score = bowlingMethod(input);
        int score = slidingWindowsScore(input);
        if(score == -1){
            cout << "Error occured, try again" << endl;
        }
        else{
            cout << "Summed score: "<<score << endl;         
        }
    }
}