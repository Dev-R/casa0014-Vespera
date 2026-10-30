/*
 * votes.ino
 *
 * Purpose:
 * Stores and manages mood votes.
 *
 * Functionality:
 * - Records new votes.
 * - Counts active votes for each mood.
 * - Removes votes after the fade period.
 * - Ensures each mood can only display up to 6 active votes.
 *
 * Notes:
 * - voteTime[mood][slot] is the main vote storage.
 * - All LED and screen updates are based on this data.
 * - Expired votes are automatically cleared after FADE_MS.
 */

// How many live (un-faded) votes a mood has right now.
int liveVotes(int m) {
  unsigned long now = millis();
  int n = 0;
  for (int s = 0; s < VOTES_PER; s++)
    if (voteTime[m][s] != 0 && now - voteTime[m][s] < FADE_MS) n++;
  return n;
}

// Clear any vote older than FADE_MS. Returns true if anything was cleared,
// so the caller knows to redraw the dome.
bool expireOld() {
  unsigned long now = millis();
  bool changed = false;
  for (int m = 0; m < N_MOODS; m++)
    for (int s = 0; s < VOTES_PER; s++)
      if (voteTime[m][s] != 0 && now - voteTime[m][s] >= FADE_MS) {
        voteTime[m][s] = 0;
        changed = true;
      }
  return changed;
}

// Add a vote to mood m in the first free (or already-expired) slot.
// Returns false if all 6 slots are live - the "hold at 6" rule.
bool addVote(int m) {
  for (int s = 0; s < VOTES_PER; s++)
    if (voteTime[m][s] == 0 || millis() - voteTime[m][s] >= FADE_MS) {
      voteTime[m][s] = millis();
      return true;
    }
  return false;
}
