#pragma once

// Runs the world headless so agents driven by another process can live in it.
// Reads one JSON command per line on stdin and answers each with one JSON line
// on stdout. Nothing is rendered and no window is opened. The protocol is
// described in agents/README.md.
int run_agent_server();
