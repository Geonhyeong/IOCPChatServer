using System;
using System.Text;

public class ChatMessageGenerator
{
    private static readonly string[] Words = { "Hello", "Hi", "How are you?", "Nice to meet you", "What's up?", "I'm good", "Yes", "No", "Really?", "That's interesting", "Tell me more" };
    private static readonly Random Random = new Random();

    public static string GenerateChatMessage()
    {
        StringBuilder sb = new StringBuilder();
        int sentenceCount = 1;

        for (int i = 0; i < sentenceCount; i++)
        {
            sb.Append(GenerateSentence());
            sb.Append(" ");
        }

        return sb.ToString().Trim();
    }

    private static string GenerateSentence()
    {
        StringBuilder sb = new StringBuilder();
        int wordCount = Random.Next(1, 3);

        for (int i = 0; i < wordCount; i++)
        {
            string word = Words[Random.Next(Words.Length)];
            sb.Append(word);
            sb.Append(" ");
        }

        sb.Append(GetPunctuation());
        return sb.ToString().Trim();
    }

    private static string GetPunctuation()
    {
        string[] punctuations = { ".", ".", ".", "!", "!", "?", "...", "" };
        return punctuations[Random.Next(punctuations.Length)];
    }
}

public class RandomNicknameGenerator
{
    private static readonly string[] adjectives = { "Happy", "Brave", "Clever", "Funny", "Gentle", "Wild", "Kind", "Smart", "Witty", "Lucky" };
    private static readonly string[] nouns = { "Cat", "Dog", "Fox", "Tiger", "Lion", "Bear", "Monkey", "Elephant", "Dolphin", "Penguin" };
    private static readonly Random random = new Random();

    public static string GenerateNickname()
    {
        string adjective = adjectives[random.Next(adjectives.Length)];
        string noun = nouns[random.Next(nouns.Length)];
        return $"{adjective}{noun}";
    }
}