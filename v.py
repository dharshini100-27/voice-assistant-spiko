import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT']="1"
import speech_recognition as sr
import pygame
import sys

pygame.mixer.init()

def play_beep():
    pygame.mixer.music.load('s.mp3')
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy():
        continue

def main():
    r = sr.Recognizer()

    with sr.Microphone() as source:
        r.adjust_for_ambient_noise(source)
        print("Listening...", flush=True)

        beep_played = False

        try:
            while True:
                if not beep_played:
                    play_beep()
                    beep_played = True

                audio = r.listen(source)

                try:
                    text = r.recognize_google(audio)
                    print(text, flush=True)   
                    if 'bye' in text.lower():
                        break
                    beep_played = False

                except sr.UnknownValueError:
                    print("Could not understand", flush=True)
                except sr.RequestError as e:
                    print(f"Request error: {e}", flush=True)

        except KeyboardInterrupt:
            print("Stopped", flush=True)

if __name__ == "__main__":
    main()