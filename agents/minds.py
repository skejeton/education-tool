"""What drives the agents: Claude, or a scripted stand-in that runs offline."""

import json
import random
from dataclasses import dataclass, field

DEFAULT_MODEL = "claude-opus-5-5"
DEFAULT_EFFORT = "low"


@dataclass
class Persona:
    name: str
    role: str
    traits: str
    goal: str
    # Places the scripted mind visits in order, and what it says to people.
    routine: list = field(default_factory=list)
    lines: list = field(default_factory=list)


@dataclass
class Situation:
    """One agent's view of the town at the start of its turn."""

    clock: str
    text: str  # The full description given to the language model.
    places: list  # Places the agent can walk to.
    people: list  # Other agents it can walk to.
    nearby: list  # Agents close enough to hear it.
    heard: list  # (speaker, words) said to it since its last turn.
    at: str = None  # The place it's at, if any.
    moving: bool = False


@dataclass
class Action:
    thought: str = ""
    say: str = ""
    go_to: str = ""  # A place or a person, or "" to stay or keep walking.


def action_schema(situation):
    """The JSON schema an action must match, limited to real destinations."""
    return {
        "type": "object",
        "properties": {
            "thought": {
                "type": "string",
                "description": "Your private reasoning for this turn, one sentence.",
            },
            "say": {
                "type": "string",
                "description": "What you say out loud, or an empty string to stay quiet.",
            },
            "go_to": {
                "type": "string",
                "enum": [""] + situation.places + situation.people,
                "description": "Where to walk: a place or a person. An empty string "
                "stays put, or keeps walking to where you're already going.",
            },
        },
        "required": ["thought", "say", "go_to"],
        "additionalProperties": False,
    }


SYSTEM_PROMPT = """You are {name}, {role} in a small town. {traits}

Your goal today: {goal}

You live in a simulation. Every turn is 10 minutes of town time: you're told \
where you are, who is around and what you heard, and you choose one action. \
You can walk to a place or to a person, and you can say something out loud; \
people within {hearing} tiles hear you. Walking across town takes a few turns.

Stay in character. Pursue your goal by meeting people and talking to them. \
Keep what you say to one or two short sentences, react to what others say, \
and don't repeat yourself. Only talk to people who are close enough to hear you."""


class ClaudeMind:
    """Decides each turn with one Claude request returning a structured action."""

    def __init__(self, model=DEFAULT_MODEL, effort=DEFAULT_EFFORT, hearing=6, client=None):
        if client is None:
            import anthropic

            client = anthropic.Anthropic()
        self.client = client
        self.model = model
        self.effort = effort
        self.hearing = hearing
        self.input_tokens = 0
        self.output_tokens = 0

    def decide(self, persona, situation):
        response = self.client.beta.messages.create(
            model=self.model,
            max_tokens=16000,
            # On a safety decline, the API retries on its recommended
            # fallback model instead of returning the refusal.
            betas=["server-side-fallback-2026-07-01"],
            fallbacks="default",
            system=SYSTEM_PROMPT.format(
                name=persona.name,
                role=persona.role,
                traits=persona.traits,
                goal=persona.goal,
                hearing=self.hearing,
            ),
            messages=[{"role": "user", "content": situation.text}],
            output_config={
                "effort": self.effort,
                "format": {"type": "json_schema", "schema": action_schema(situation)},
            },
        )
        self.input_tokens += response.usage.input_tokens
        self.output_tokens += response.usage.output_tokens

        if response.stop_reason in ("refusal", "max_tokens"):
            return Action(thought=f"(no decision: {response.stop_reason})")

        text = next(block.text for block in response.content if block.type == "text")
        data = json.loads(text)
        return Action(data["thought"], data["say"].strip(), data["go_to"])

    def usage(self):
        return f"{self.input_tokens} input and {self.output_tokens} output tokens"


class ScriptedMind:
    """Follows each persona's routine and greets people it meets.

    Needs no network or API key, so the demo and tests run anywhere.
    """

    def __init__(self, seed=0):
        self.rng = random.Random(seed)
        self.stops = {}  # Agent -> index into its routine.
        self.waited = {}  # Agent -> turns spent at the current stop.
        self.greeted = {}  # Agent -> people it already talked to.

    def decide(self, persona, situation):
        name = persona.name
        stop = self.stops.setdefault(name, 0)
        greeted = self.greeted.setdefault(name, set())
        action = Action()

        if situation.heard:
            speaker = situation.heard[-1][0]
            action.say = self.rng.choice([f"Good to see you, {speaker}!", f"Sounds good, {speaker}.", "Ha, really?"])
            greeted.add(speaker)
        else:
            strangers = [person for person in situation.nearby if person not in greeted]
            if strangers and persona.lines:
                other = strangers[0]
                action.say = f"Hi {other}! " + self.rng.choice(persona.lines)
                greeted.add(other)

        routine = [place for place in persona.routine if place in situation.places]
        if not situation.moving and routine:
            target = routine[stop % len(routine)]
            if situation.at == target:
                self.waited[name] = self.waited.get(name, 0) + 1
                if self.waited[name] >= 2:
                    self.stops[name] = stop + 1
                    self.waited[name] = 0
                    action.go_to = routine[(stop + 1) % len(routine)]
                    action.thought = f"Time to head to {action.go_to}."
            else:
                action.go_to = target
                action.thought = f"I should go to {target}."

        return action

    def usage(self):
        return "no tokens (scripted)"
