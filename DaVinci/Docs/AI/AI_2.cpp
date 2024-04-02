// Listing 1

Monster::PlayerSpotted(bool hear_player, bool see_player, float* pos)
{
    if(!see_player)
    {
        if(hear_player)
        {
            //we have heard something..check out
            //situation..keep an eye on the place
            FocusAttention(pos); //native function
        }
        else
        {
            //someone told us about the
            //player (monster friend or so)
            //let's help our friends
            WalkTo(pos); //native function
        }
    }
    else
    {
        //we can see the enemy..let's kill him!
        ShootAt(pos); //native function
    }
}




// Listing 2

Monster::PlayerSpotted(bool hear_player=true,
                       bool see_player=false,
                       float* pos)
{
    FocusAttention(pos);
}


