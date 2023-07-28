# PPM IMAGE FORMATTER
A simple C program that allows you to modify images with a .ppm extension (only p3 for now).

Compile and run the program in the terminal with one of the options:

```python 
# main <path to your input ppm image> -<option> <optional parameter>

-blackwhite

# 0.5 - no changes, < 0.5 - decrease gamma, > 0.5 - increase gamma    
-gamma <intensity (value must be between 0 - 1)>

#  eg. r - only red color, rgb - all colors visible    
-filter <available filters (rgb) and all theirs combinations>

# 0.5 - no changes, < 0.5 - decrease gamma, > 0.5 - increase contrast       
-contrast <intensity (value must be between 0 - 1)>
    
```

## Example usage

  *Original Photo:*
  ![image](https://github.com/julgitt/PPM-Image-Formatter/assets/95649808/e85116b7-8154-4ef0-98a6-f0720be00d08)

  *Black and white filter:*
  ![image](https://github.com/julgitt/PPM-Image-Formatter/assets/95649808/48020370-e207-414f-afbc-be4947d36fd9)

  *Filter with rb option:*
  ![image](https://github.com/julgitt/PPM-Image-Formatter/assets/95649808/c73ecc3c-b209-4a5d-a2cd-aa29cf417ffa)

  *Gamma with 0.8 value:*
  ![image](https://github.com/julgitt/PPM-Image-Formatter/assets/95649808/27c9cf90-f598-4e47-8c9d-079806d03d5d)

  *Contrast with 0.3 value:*
  ![image](https://github.com/julgitt/PPM-Image-Formatter/assets/95649808/b9246d99-3df4-4889-aa1d-2e44a10c389a)


