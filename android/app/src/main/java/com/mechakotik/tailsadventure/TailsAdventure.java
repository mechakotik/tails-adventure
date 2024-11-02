package com.mechakotik.tailsadventure;
import org.libsdl.app.SDLActivity;

public class TailsAdventure extends SDLActivity {
    protected String[] getLibraries() {
        return new String[] {
                "SDL3",
                "tails-adventure"
        };
    }
}
