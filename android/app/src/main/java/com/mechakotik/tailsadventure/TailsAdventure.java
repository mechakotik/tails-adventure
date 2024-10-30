package com.mechakotik.tailsadventure;
import org.libsdl.app.SDLActivity;

public class TailsAdventure extends SDLActivity {
    protected String[] getLibraries() {
        return new String[] {
                "SDL3",
                "SDL3_image",
                "SDL3_mixer",
                "tails-adventure"
        };
    }
}
