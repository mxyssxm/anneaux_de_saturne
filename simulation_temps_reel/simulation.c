/*
 * Simulation des anneaux de Saturne – visualisation temps réel SDL2
 * 10 astéroïdes en orbite autour de Saturne + 1 satellite perturbateur
 * Intégration Runge-Kutta 4
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

/* ── Constantes physiques ────────────────────────────────────────────────── */
#define G           6.67430e-20   /* km³ / (kg·s²)                           */
#define MS          5.683e26      /* masse de Saturne  [kg]                  */
#define M_SAT       1.3452e23     /* masse de Titan [kg]                     */

/* ── Simulation ──────────────────────────────────────────────────────────── */
#define DT          10.0          /* pas de temps [s]                        */
#define STEPS_PER_FRAME 100       /* itérations physiques par frame          */
#define NUM_ASTEROIDS 10

/* ── Fenêtre ─────────────────────────────────────────────────────────────── */
#define WIN_W  900
#define WIN_H  900
#define SCALE  0.000325           /* pixels / km  – zoom pour inclure Titan  */

/* ── Couleurs ────────────────────────────────────────────────────────────── */
#define COL_BG      255, 255, 255, 255   /* fond blanc                       */
#define COL_SATURN  220, 160,   0, 255   /* jaune-doré                       */
#define COL_SAT      30, 100, 220, 255   /* satellite bleu                   */
#define COL_AST       80,  80,  80, 255  /* astéroïdes gris foncé            */
#define COL_TRAIL    100, 100, 100,  80  /* traînée grise transparente       */

#define SATURN_R    10   /* rayon visuel de Saturne en pixels               */
#define SAT_R        5
#define AST_R        3

#define TRAIL_LEN  400   /* longueur de la traînée (nb de points)           */

/* ── Vecteur 2D ──────────────────────────────────────────────────────────── */
typedef struct { double x, y; } V2;

/* ── Objet simulé ────────────────────────────────────────────────────────── */
typedef struct {
    V2 pos, vel;
    V2 trail[TRAIL_LEN];
    int trail_head;
    int trail_count;
    Uint8 r, g, b;
} Body;

/* ── Accélération subie par un corps (gravité Saturne + satellite optionnel) */
static V2 accel(V2 pos, V2 sat_pos, int include_satellite) {
    V2 a = {0, 0};

    /* Saturne */
    double rs = sqrt(pos.x*pos.x + pos.y*pos.y);
    if (rs > 1e-3) {
        double f = -G * MS / (rs * rs * rs);
        a.x += f * pos.x;
        a.y += f * pos.y;
    }

    /* Satellite */
    if (include_satellite) {
        double dx = sat_pos.x - pos.x;
        double dy = sat_pos.y - pos.y;
        double r = sqrt(dx*dx + dy*dy);
        if (r > 1e-3) {
            double f = G * M_SAT / (r * r * r);
            a.x += f * dx;
            a.y += f * dy;
        }
    }

    return a;
}

/* ── Runge-Kutta 4 ───────────────────────────────────────────────────────── */
static void rk4(Body *b, V2 sat_pos, double dt, int is_asteroid) {
    V2 p = b->pos, v = b->vel;

    V2 k1v = accel(p, sat_pos, is_asteroid);
    V2 k1r = v;

    V2 tp = {p.x + 0.5*dt*k1r.x, p.y + 0.5*dt*k1r.y};
    V2 tv = {v.x + 0.5*dt*k1v.x, v.y + 0.5*dt*k1v.y};
    V2 k2v = accel(tp, sat_pos, is_asteroid);
    V2 k2r = tv;

    tp.x = p.x + 0.5*dt*k2r.x; tp.y = p.y + 0.5*dt*k2r.y;
    tv.x = v.x + 0.5*dt*k2v.x; tv.y = v.y + 0.5*dt*k2v.y;
    V2 k3v = accel(tp, sat_pos, is_asteroid);
    V2 k3r = tv;

    tp.x = p.x + dt*k3r.x; tp.y = p.y + dt*k3r.y;
    tv.x = v.x + dt*k3v.x; tv.y = v.y + dt*k3v.y;
    V2 k4v = accel(tp, sat_pos, is_asteroid);
    V2 k4r = tv;

    b->pos.x = p.x + (dt/6.0)*(k1r.x + 2*k2r.x + 2*k3r.x + k4r.x);
    b->pos.y = p.y + (dt/6.0)*(k1r.y + 2*k2r.y + 2*k3r.y + k4r.y);
    b->vel.x = v.x + (dt/6.0)*(k1v.x + 2*k2v.x + 2*k3v.x + k4v.x);
    b->vel.y = v.y + (dt/6.0)*(k1v.y + 2*k2v.y + 2*k3v.y + k4v.y);
}

/* ── Coordonnées monde → pixels ──────────────────────────────────────────── */
static inline int wx(double x) { return (int)(WIN_W/2 + x * SCALE); }
static inline int wy(double y) { return (int)(WIN_H/2 - y * SCALE); }

/* ── Remplir un disque ───────────────────────────────────────────────────── */
static void fill_circle(SDL_Renderer *rend, int cx, int cy, int r) {
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx*dx + dy*dy <= r*r)
                SDL_RenderDrawPoint(rend, cx+dx, cy+dy);
}

/* ── Ajouter un point à la traînée ───────────────────────────────────────── */
static void trail_push(Body *b) {
    b->trail[b->trail_head] = b->pos;
    b->trail_head = (b->trail_head + 1) % TRAIL_LEN;
    if (b->trail_count < TRAIL_LEN) b->trail_count++;
}

/* ── Dessiner la traînée ─────────────────────────────────────────────────── */
static void trail_draw(SDL_Renderer *rend, Body *b) {
    for (int i = 0; i < b->trail_count; i++) {
        int idx = (b->trail_head - b->trail_count + i + TRAIL_LEN) % TRAIL_LEN;
        /* opacité progressive */
        int alpha = (int)(255.0 * i / b->trail_count);
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rend, b->r, b->g, b->b, (Uint8)alpha);
        SDL_RenderDrawPoint(rend, wx(b->trail[idx].x), wy(b->trail[idx].y));
    }
}

typedef struct {
    char c;
    Uint8 rows[7];
} Glyph;

static const Glyph FONT[] = {
    {' ', {0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {':', {0x00,0x04,0x00,0x00,0x04,0x00,0x00}},
    {'0', {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}},
    {'1', {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}},
    {'2', {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}},
    {'3', {0x1E,0x01,0x01,0x06,0x01,0x01,0x1E}},
    {'4', {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}},
    {'5', {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}},
    {'6', {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}},
    {'7', {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}},
    {'8', {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}},
    {'9', {0x0E,0x11,0x11,0x0F,0x01,0x02,0x1C}},
    {'A', {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}},
    {'D', {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E}},
    {'E', {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}},
    {'H', {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}},
    {'I', {0x1F,0x04,0x04,0x04,0x04,0x04,0x1F}},
    {'J', {0x1F,0x02,0x02,0x02,0x12,0x12,0x0C}},
    {'L', {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}},
    {'M', {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}},
    {'N', {0x11,0x19,0x15,0x13,0x11,0x11,0x11}},
    {'O', {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}},
    {'P', {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}},
    {'R', {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}},
    {'S', {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}},
    {'T', {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}},
    {'U', {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}}
};

static const Uint8 *glyph_rows(char c) {
    size_t count = sizeof(FONT) / sizeof(FONT[0]);
    for (size_t i = 0; i < count; i++)
        if (FONT[i].c == c)
            return FONT[i].rows;
    return FONT[0].rows;
}

static void draw_text(SDL_Renderer *rend, int x, int y, const char *text, int scale,
    Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(rend, r, g, b, 255);

    for (int k = 0; text[k] != '\0'; k++) {
        const Uint8 *rows = glyph_rows(text[k]);
        for (int row = 0; row < 7; row++) {
            for (int col = 0; col < 5; col++) {
                if ((rows[row] >> (4 - col)) & 1) {
                    SDL_Rect px = {
                        x + k * 6 * scale + col * scale,
                        y + row * scale,
                        scale,
                        scale
                    };
                    SDL_RenderFillRect(rend, &px);
                }
            }
        }
    }
}

static void format_sim_time(double seconds, char *buffer, size_t size) {
    long total = (long)(seconds + 0.5);
    long days = total / 86400;
    long hours = (total % 86400) / 3600;
    long minutes = (total % 3600) / 60;
    long secs = total % 60;
    snprintf(buffer, size, "TEMPS %02ldJ:%02ldH:%02ldM:%02ldS", days, hours, minutes, secs);
}

static void draw_legend(SDL_Renderer *rend, double sim_time) {
    SDL_Rect panel = {20, 20, 310, 118};
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 220);
    SDL_RenderFillRect(rend, &panel);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(rend, 60, 60, 60, 255);
    SDL_RenderDrawRect(rend, &panel);

    char time_text[64];
    format_sim_time(sim_time, time_text, sizeof(time_text));
    draw_text(rend, 32, 30, time_text, 2, 30, 30, 30);

    SDL_SetRenderDrawColor(rend, 220, 160, 0, 255);
    fill_circle(rend, 38, 68, 6);
    draw_text(rend, 55, 61, "SATURNE", 2, 30, 30, 30);

    SDL_SetRenderDrawColor(rend, 80, 80, 80, 255);
    fill_circle(rend, 38, 94, 4);
    draw_text(rend, 55, 87, "ASTEROIDES", 2, 30, 30, 30);

    SDL_SetRenderDrawColor(rend, 210, 50, 50, 255);
    fill_circle(rend, 38, 120, 5);
    draw_text(rend, 55, 113, "SATELLITE", 2, 30, 30, 30);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
int main(void) {
    srand((unsigned)time(NULL));

    /* ── Initialisation SDL ── */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow(
        "Anneaux de Saturne – simulation temps réel",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }

    SDL_Renderer *rend = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!rend) { fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError()); return 1; }

    /* ── Satellite : Titan ── */
    Body satellite = {0};
    satellite.pos = (V2){1221870.0, 0.0};  /* distance réelle de Titan à Saturne */
    double vsat = sqrt(G * MS / 1221870.0); /* ~5.57 km/s en orbite circulaire    */
    satellite.vel = (V2){0.0, vsat};
    satellite.r = 210; satellite.g = 50; satellite.b = 50;  /* rouge */

    /* ── Astéroïdes en orbites circulaires stables ── */
    Body asteroids[NUM_ASTEROIDS] = {0};
    /* couleurs pastel variées */
    Uint8 colors[NUM_ASTEROIDS][3] = {
        {200, 80,  0},{  0,140, 80},{ 120, 0,200},{180,140,  0},
        {  0,100,200},{200,  0,120},{ 80,170,  0},{140, 60,200},
        {200,120,  0},{  0,160,160}
    };
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        double dist  = 100000.0 + i * 15000.0;          /* 100 000 – 235 000 km  */
        double angle = ((double)rand() / RAND_MAX) * 2.0 * M_PI;
        double speed = sqrt(G * MS / dist);              /* vitesse circulaire     */
        asteroids[i].pos = (V2){dist*cos(angle), dist*sin(angle)};
        asteroids[i].vel = (V2){-speed*sin(angle), speed*cos(angle)};
        asteroids[i].r = colors[i][0];
        asteroids[i].g = colors[i][1];
        asteroids[i].b = colors[i][2];
    }

    /* ── Boucle principale ── */
    int running = 1;
    SDL_Event ev;
    int frame = 0;
    double sim_time = 0.0;

    while (running) {
        /* Events */
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }

        /* Physique */
        for (int s = 0; s < STEPS_PER_FRAME; s++) {
            V2 sp = satellite.pos;
            for (int i = 0; i < NUM_ASTEROIDS; i++)
                rk4(&asteroids[i], sp, DT, 1);
            rk4(&satellite, (V2){0,0}, DT, 0);
        }
        sim_time += DT * STEPS_PER_FRAME;

        /* Traînée toutes les 3 frames */
        if (frame % 3 == 0) {
            trail_push(&satellite);
            for (int i = 0; i < NUM_ASTEROIDS; i++)
                trail_push(&asteroids[i]);
        }
        frame++;

        /* ── Rendu ── */
        /* fond */
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(rend, COL_BG);
        SDL_RenderClear(rend);

        /* traînées */
        trail_draw(rend, &satellite);
        for (int i = 0; i < NUM_ASTEROIDS; i++)
            trail_draw(rend, &asteroids[i]);

        /* Saturne */
        SDL_SetRenderDrawColor(rend, 220, 160, 0, 255);
        fill_circle(rend, WIN_W/2, WIN_H/2, SATURN_R);

        /* Satellite */
        SDL_SetRenderDrawColor(rend, satellite.r, satellite.g, satellite.b, 255);
        fill_circle(rend, wx(satellite.pos.x), wy(satellite.pos.y), SAT_R);

        /* Astéroïdes */
        for (int i = 0; i < NUM_ASTEROIDS; i++) {
            SDL_SetRenderDrawColor(rend,
                asteroids[i].r, asteroids[i].g, asteroids[i].b, 255);
            fill_circle(rend,
                wx(asteroids[i].pos.x), wy(asteroids[i].pos.y), AST_R);
        }

        draw_legend(rend, sim_time);
        SDL_RenderPresent(rend);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
