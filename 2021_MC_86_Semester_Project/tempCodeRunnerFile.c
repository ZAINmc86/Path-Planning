        if (rand_gene[i][0] != 0)
        {
            turns++;
        }
        for (int j = 0; j < size - 1; j++) 
        {
            if (rand_gene[i][j] != rand_gene[i][j + 1])
            {
                turns++;
            }
        }
        if (rand_gene[i][size - 1] != (path_size - 1)) 
        {
            turns++;
        }
        path_turns[i] = turns;
        turns = 0;