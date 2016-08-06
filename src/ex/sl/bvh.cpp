#include "bvh.h"


Skel *BVH::LoadFile(char *filename)
{
  char buff[1024], token[1024], token2[1024];
  char arg[10][100];
  int retval, depth=0;
  Channel *the_channel;
  CHANNEL_LIST channels;

  Skel *the_skel, *current_skel;
  SKEL_STACK skel_stack;

  thefile = new ifstream(filename);

  if (! thefile) {
    return NULL;
  }

  state = 0;
  while (thefile->getline(buff, 1024) ) {

//    cout << "STATE IS " << state << " and buff is "<< buff <<endl;

    switch (state) {
    case 0: { /* we're at the top */

      retval = sscanf (buff, "%s", token);

      if (retval) {
        //cout << "Retval was "<<retval << " and token is " << token << endl;
        state = 1;
        if (!strcmp("HIERARCHY", token)) {
          state = 1;
          //cout << "Switched state." << endl;
        } else {
          //cout << "Saw " << token << endl;
        }
      }

      break;}
    case 1: { /* in the heirachy - looking for ROOT name */
      retval = sscanf (buff, "%s %s", token, token2);

      if (retval) {
        //cout << "Retval was "<<retval << " and token is " << token << " and token2 is " << token2 << endl;
        state = 1;
        if (!strcmp("ROOT", token)) {
          // Make the skel

          the_skel = new Skel();
          current_skel = the_skel;
          strcpy(the_skel->name, token2 );

          state = 2;
          //cout << "Switched state." << endl;
        } else {
          //cout << "Saw " << token << endl;
        }
      }
      break;}

    case 2: { /* we're in the skel */
      retval = sscanf (buff, "%s %s %s %s %s %s %s %s %s %s",
        arg[0], arg[1], arg[2], arg[3], arg[4],
        arg[5], arg[6], arg[7], arg[8], arg[9]);

      //cout << "Retval is " << retval << " and args are: ";
      for (int i = 0; i < retval; i++) {
        //cout << arg[i] << " ";
      }

      //cout << endl;

      if (arg[0][0] == '{') {
        //cout << "LCURLEY at depth "<<depth<<"\n";

      } else if (arg[0][0] == '}') {
        //cout << "RCURLEY at depth "<<depth<<"\n";
        depth--;
        //cout << "depth is now "<<depth<<endl;
        if (depth >=0 ) {
          the_skel = skel_stack.front();
          skel_stack.pop_front();
        } else {
          state++;
        }
        //cout << "MY NAME IS NOW " << the_skel->name << endl;
      } else if (!strcmp(arg[0], "OFFSET")) {
        //cout << "OFFSET!\n";
        the_skel->off_x = atof(arg[1]);
        the_skel->off_y = atof(arg[2]);
        the_skel->off_z = atof(arg[3]);
      } else if (!strcmp(arg[0], "CHANNELS")) {
        int numchannels = atoi(arg[1]);
        //cout << "CHANNELS\n";
        //cout << "I'll need to create " << numchannels << " of them.\n";

        for (int j = 2; j < 2 + numchannels; j++ ) {
          the_channel = new Channel();
          if (arg[j][0] == 'X') {
            if (arg[j][1] == 'p') {
              the_channel->type = X_TRANSLATION;
            } else
            if (arg[j][1] == 'r') {
              the_channel->type = X_ROTATION;
            }
          } else
          if (arg[j][0] == 'Y') {
            if (arg[j][1] == 'p') {
              the_channel->type = Y_TRANSLATION;
            } else
            if (arg[j][1] == 'r') {
              the_channel->type = Y_ROTATION;
            }
          } else
          if (arg[j][0] == 'Z') {
            if (arg[j][1] == 'p') {
              the_channel->type = Z_TRANSLATION;
            } else
            if (arg[j][1] == 'r') {
              the_channel->type = Z_ROTATION;
            }
          }
          channels.push_back(the_channel);
          the_skel->AddChannel(the_channel);
        }

      } else if (!strcmp(arg[0], "JOINT")) {
        skel_stack.push_front(the_skel);

        the_skel = new Skel();
        strcpy(the_skel->name, arg[1]);
        (skel_stack.front())->AddChild(the_skel);

        depth++;
      } else if (!strcmp(arg[0], "End")) {
        skel_stack.push_front(the_skel);

        the_skel = new Skel();
        strcpy(the_skel->name, "End Site");
        (skel_stack.front())->AddChild(the_skel);

        depth++;
      } else if (!strcmp(arg[0], "MOTION")) {
        abort();
      }
      break; }

    case 3: { /* we're in the MOTION section */
      if (!strcmp(buff, "MOTION")) {
      } else {
        sscanf(buff, "%s %s %s", arg[0], arg[1], arg[2]);
        if (!strcmp(arg[0], "Frames:")) {
          the_skel->num_frames = atoi(arg[1]);
        } else if (!strcmp(arg[0], "Frame") && !strcmp(arg[1], "Time:")) {
          the_skel->frame_time = atof(arg[2]);
          state++;
        }

      }
      break; }

    case 4: { /* we're in the MOTION DATA section */
      strcpy(token, buff);
      char *cp = token;
      cp = strtok(cp, "\t ");
      for (CHANNEL_LIST::iterator j = channels.begin(); j != channels.end(); ++j) {
        Channel *this_channel;
        if (!cp) {
          //cout << "Error reading motion data?\n";

          abort();
        } else {
          this_channel = *j;
          (*j)->AddFrame(atof(cp));
          (*j)->Reset();
        }

        cp = strtok(NULL, "\t ");

      }
      break; }

    default: {
      cout << "Thats odd... \n";
      abort();
      }
    }
  }

  cout << "BVH -- FILE LOADED.\n";
  return the_skel;
}
