%orig_image = imread('wallpaper.bmp', 'bmp');
%orig_image = imread('test2', 'bmp');
orig_image  = imread('fb', 'bmp');

bnw = rgb2gray(orig_image);
imwrite(bnw, 'grayscale.jpg', 'jpg');

for i = 1:1080
    for j = 1:1920
        
        if (mod(i, 2) == 1 && mod(j, 2) == 1) % red pixel
              orig_image(i, j, 2) = bitand(orig_image(i, j, 2), 0);
              orig_image(i, j, 3) = bitand(orig_image(i, j, 3), 0); 

        elseif (mod(i, 2) == 0 && mod(j, 2) == 0) % blue pixel
            orig_image(i, j, 1) = bitand(orig_image(i, j, 1), 0);
            orig_image(i, j, 2) = bitand(orig_image(i, j, 2), 0);
            
        else % green pixel
            orig_image(i, j, 1) = bitand(orig_image(i, j, 1), 0);
            orig_image(i, j, 3) = bitand(orig_image(i, j, 3), 0);
          
        end
    end
end

image(orig_image);

imwrite(orig_image, 'bayer_image.jpg', 'jpg');
bayer_image = orig_image;          


if (i == 1)
    
end

for i = 1:1080
    for j = 1:1920
        
        i_minus_1 = i-1;
        i_plus_1 = i+1;
        j_minus_1 = j-1;
        j_plus_1 = j+1;
        
        if (i == 1)
            if(j == 1)              %top left corner
                i_minus_1 = i+1;
				j_minus_1 = j+1;
            elseif (j == 1920)      %top right corner
                i_minus_1 = i+1;
				j_plus_1 = j-1;
            else                    %top row except the corners
                i_minus_1 = i+1;
            end
           
        elseif (i == 1080)
            if(j == 1)              %bottom left corner
				j_minus_1 = j+1;
				i_plus_1 = i-1;
			
            elseif(j == 1920) 		%bottom right corner
				i_plus_1 = i-1;
				j_plus_1 = j-1;
                
            else
				i_plus_1 = i-1;		%bottom row without the corners
            end
            
        elseif (j == 1 && i > 1 && i < 1080)
            j_minus_1 = j+1;		%left column without the corners
        
        elseif (j == 1920 && i > 1 && i < 1080)
            j_plus_1 = j-1;			%right column without the corners
        
        end
        
        if (mod(i, 2) == 1 && mod(j, 2) == 1)                                % red pixel
            bayer_image(i, j, 2) = (orig_image(i_plus_1, j, 2)...            %green component at neighbors
                                   + orig_image(i_minus_1, j, 2)...
                                   + orig_image(i, j_minus_1, 2)...
                                   + orig_image(i, j_plus_1, 2))/4;          
            bayer_image(i, j, 3) = (orig_image(i_minus_1, j_minus_1, 3)...   %blue components at corners
                                   + orig_image(i_plus_1, j_minus_1, 3)...
                                   + orig_image(i_minus_1, j_plus_1, 3)...
                                   + orig_image(i_plus_1, j_plus_1, 3))/4;   
            
        elseif (mod(i, 2) == 0 && mod(j, 2) == 0)   % blue pixel
            bayer_image(i, j, 1) = (orig_image(i_minus_1, j_minus_1, 1)...   %red components at corners
                                   + orig_image(i_plus_1, j_minus_1, 1)...
                                   + orig_image(i_minus_1, j_plus_1, 1)...
                                   + orig_image(i_plus_1, j_plus_1, 1))/4;   
            bayer_image(i, j, 2) = (orig_image(i_plus_1, j, 2)...            %green components at neighbors
                                   + orig_image(i_minus_1, j, 2)...
                                   + orig_image(i, j_minus_1, 2)...
                                   + orig_image(i, j_plus_1, 2))/4;          
        
        elseif (mod(i, 2) == 1 && mod(j, 2) == 0)   %green pixel with blue on left and right
            bayer_image(i, j, 1) = (orig_image(i_minus_1, j, 1)...           %red components top and bottom
                                  + orig_image(i_plus_1, j, 1))/2;           
            bayer_image(i, j, 3) = (orig_image(i, j_minus_1, 3)...           %blue components left and right
                                  + orig_image(i, j_plus_1, 3))/2;           
            
        else                                        %green pixel with red on left and right
            bayer_image(i, j, 1) = (orig_image(i, j_minus_1, 1)...           %red components left and right
                                  + orig_image(i, j_plus_1, 1))/2;           
            bayer_image(i, j, 3) = (orig_image(i_minus_1, j, 3)...           %blue components top and bottom
                                  + orig_image(i_plus_1, j, 3))/2;           
           
        end
    end
end

%image(bayer_image);
imwrite(bayer_image, 'RBG_image.jpg', 'jpg');


YCbCr_image = bayer_image;

YCbCr_conversion_matrix = [0.183 0.614 0.062; -0.101 -0.338 0.439; 0.439 -0.399 -0.040];
YCbCr_conversion_constants = [16; 128; 128];

for i = 1:1080
    for j = 1:1920
        rgb_matrix = double([bayer_image(i, j, 1); bayer_image(i, j, 2); bayer_image(i, j, 3)]);
        YCbCr_image(i, j, :) = uint8(YCbCr_conversion_matrix * rgb_matrix + YCbCr_conversion_constants);
        
    end
end

%image(YCbCr_image)
imwrite(YCbCr_image, 'YCbCr_image.jpg', 'jpg')

