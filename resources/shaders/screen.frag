#version 330 core
in vec2 uv_vert;
uniform sampler2D screenTexture;
uniform int postpro;
uniform float iheight;
uniform float iwidth;
out vec4 fragColor;

void main()
{
    fragColor = texture(screenTexture,uv_vert);
    // invert
    if (postpro == 1) {
        fragColor = vec4(1.0f) - fragColor;

    // grayscale
    } else if (postpro == 2) {
        float gray = 0.299 * fragColor[0] + 0.587 * fragColor[1] + 0.114 * fragColor[2];
        fragColor = vec4(gray,gray,gray,1.0f);

    // blur
    } else if (postpro == 3) {
        float kernel[25];
        for (int i = 0; i < 25; i++){
            kernel[i] = 0.04;
        }
        vec4 sampleTex[25];
        int index = 0;
        for (int y = -2; y <= 2; y++) {
            for (int x = -2; x <= 2; x++) {
                vec2 offset = vec2(float(x) * iwidth, float(y) * iheight);
                sampleTex[index] = texture(screenTexture, uv_vert + offset);
                index++;
            }
        }
        vec4 color = vec4(0.0);
        for (int i = 0; i < 25; i++){
            color += sampleTex[i] * kernel[i];
        }
        fragColor = color;

    // shapern
    } else if (postpro == 4) {
        float kernel[9];
        for (int i = 0; i < 9; i++){
            if (i == 4) kernel[i] = 17.0f / 9.0f;
            else kernel[i] = -1.0f/ 9.0f;
        }
        vec4 sampleTex[9];
        int index = 0;
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                vec2 offset = vec2(float(x) * iwidth, float(y) * iheight);
                sampleTex[index] = texture(screenTexture, uv_vert + offset);
                index++;
            }
        }
        vec4 color = vec4(0.0);
        for (int i = 0; i < 9; i++){
            color += sampleTex[i] * kernel[i];
        }
        fragColor = color;

    // tone mapping
    } else if (postpro == 5) {
        float gamma = 0.5f;
        float r = clamp(pow(fragColor[0], gamma),0,1);
        float g = clamp(pow(fragColor[1], gamma),0,1);
        float b = clamp(pow(fragColor[2], gamma),0,1);
        fragColor = vec4(r,g,b,1);

    // edge detected
    } else if (postpro == 6) {
        float kernelX[9] = float[](
                    -1.0,  0.0,  1.0,
                    -2.0,  0.0,  2.0,
                    -1.0,  0.0,  1.0);
        float kernelY[9] = float[](
                    -1.0, -2.0, -1.0,
                     0.0,  0.0,  0.0,
                     1.0,  2.0,  1.0);
        float sumX = 0.0;
        float sumY = 0.0;
        int index = 0;
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                vec2 offset = vec2(float(x) * iwidth, float(y) * iheight);
                vec4 sampleColor = texture(screenTexture, uv_vert + offset);
                float gray = 0.299 * sampleColor[0] + 0.587 * sampleColor[1] + 0.114 * sampleColor[2];
                sumX += gray * kernelX[index];
                sumY += gray * kernelY[index];
                index++;
            }
        }
        float gradient = clamp(length(vec2(sumX, sumY)),0,1);
        fragColor = vec4(gradient,gradient,gradient,1);
    }
    // FXAA
    else if (postpro == 7) {
        vec2 inverseVP = vec2(iwidth, iheight);
        vec3 rgbNW = texture(screenTexture, uv_vert + vec2(-1.0, -1.0) * inverseVP).rgb;
        vec3 rgbNE = texture(screenTexture, uv_vert + vec2(1.0, -1.0) * inverseVP).rgb;
        vec3 rgbSW = texture(screenTexture, uv_vert + vec2(-1.0, 1.0) * inverseVP).rgb;
        vec3 rgbSE = texture(screenTexture, uv_vert + vec2(1.0, 1.0) * inverseVP).rgb;
        vec3 rgbM  = fragColor.rgb;

        float lumaNW = dot(rgbNW, vec3(0.299, 0.587, 0.114));
        float lumaNE = dot(rgbNE, vec3(0.299, 0.587, 0.114));
        float lumaSW = dot(rgbSW, vec3(0.299, 0.587, 0.114));
        float lumaSE = dot(rgbSE, vec3(0.299, 0.587, 0.114));
        float lumaM  = dot(rgbM,  vec3(0.299, 0.587, 0.114));

        // Compute minimum and maximum luma
        float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
        float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
        float lumaRange = lumaMax - lumaMin;

        // Threshold for edge detection
        if (lumaRange < 0.1) {
            fragColor = vec4(rgbM, 1.0);
        } else {
            // Compute edge direction
            vec2 dir;
            dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
            dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

            // Direction Reduction: Adjust the edge direction to prevent over-smoothing and limit the sampling area.
            float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * 0.5), 0.01);
            float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
            dir = clamp(dir * rcpDirMin, vec2(-8.0), vec2(8.0)) * inverseVP;

            // Perform anti-aliasing by sampling along the edge
            vec3 rgbA = 0.5 * (
                texture(screenTexture, uv_vert + dir * (1.0 / 3.0 - 0.5)).rgb +
                texture(screenTexture, uv_vert + dir * (2.0 / 3.0 - 0.5)).rgb
            );
            vec3 rgbB = rgbA * 0.5 + 0.25 * (
                texture(screenTexture, uv_vert + dir * -0.5).rgb +
                texture(screenTexture, uv_vert + dir * 0.5).rgb
            );

            float lumaB = dot(rgbB, vec3(0.299, 0.587, 0.114));

            if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
                fragColor = vec4(rgbA, 1.0);
            } else {
                fragColor = vec4(rgbB, 1.0);
            }
        }
    }
}
