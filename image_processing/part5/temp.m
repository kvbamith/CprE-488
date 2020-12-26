





if (mod(i, 2) == 1 && mod(j, 2) == 1)                                % red pixel
    bayer_image(i, j, 2) = (orig_image(i_plus_1, j, 2)...            %green component at neighbors
                           + orig_image(i_minus_1, j, 2)...
                           + orig_image(i, j_minus_1, 2)...
                           + orig_image(i, j_plus_1, 2))/4;          
    bayer_image(i, j, 3) = (orig_image(i_minus_1, j_minus_1, 3)...   %blue components at corners
                           + orig_image(i_plus_1, j_minus_1, 3)...
                           + orig_image(i_minus_1, j_plus_1, 3)...
                           + orig_image(i_plus_1, j_plus_1, 3))/4;   