/*
 * Simulation : satellite à orbite elliptique traversant la ceinture de Saturne
 *
 * Le satellite suit une orbite elliptique :
 *   périapsis  ~80 000 km  (intérieur à la ceinture)
 *   apoapsis  ~350 000 km  (extérieur à la ceinture)
 * → il traverse les astéroïdes deux fois par orbite.
 *
 * 30 astéroïdes répartis en ceinture entre 100 000 et 230 000 km.
 * Intégration Runge-Kutta 4.
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

/* ── Constantes physiques ────────────────────────────────────────────────── */
#define G           6.67430e-20   /* km³ / (kg·s²)                           */
#define MS          5.683e26      /* masse de Saturne  [kg]                  */
#define M_SAT       8.0e23        /* masse du satellite perturbateur [kg]    */
                                  /* ~6x Titan – exagéré pour rendre l'effet visible */

/* ── Simulation ──────────────────────────────────────────────────────────── */
#define DT              10.0
#define STEPS_PER_FRAME 8
#define NUM_ASTEROIDS   30

/* ── Fenêtre ─────────────────────────────────────────────────────────────── */
#define WIN_W  900
#define WIN_H  900
#define SCALE  0.00115            /* apoapsis 350 000 km → ~402 px du centre  */

#define SATURN_R    10
#define SAT_R        6
#define AST_R        3
#define TRAIL_LEN  600

/* ── Types ───────────────────────────────────────────────────────────────── */
typedef struct { double x, y; } V2;

typedef struct {
    V2 pos, vel;
    V2 trail[TRAIL_LEN];
    int trail_head, trail_count;
    Uint8 r, g, b;
} Body;

/* ── Accélération gravitationnelle ───────────────────────────────────────── */
static V2 accel(V2 pos, V2 sat_pos, int include_satellite) {
    V2 a = {0, 0};

    double rs = sqrt(pos.x*pos.x + pos.y*pos.y);
    if (rs > 1e-3) {
        double f = -G * MS / (rs*rs*rs);
        a.x += f * pos.x;
        a.y += f * pos.y;
    }

    if (include_satellite) {
        double dx = sat_pos.x - pos.x;
        double dy = sat_pos.y - pos.y;
        double r  = sqrt(dx*dx + dy*dy);
        if (r > 500.0) {
            double f = G * M_SAT / (r*r*r);
            a.x += f * dx;
            a.y += f * dy;
        }
    }
    return a;
}

/* ── Runge-Kutta 4 ───────────────────────────────────────────────────────── */
static void rk4(Body *b, V2 sat_pos, double dt, int is_asteroid) {
    V2 p = b->pos, v = b->vel;

    V2 k1v = accel(p, sat_pos, is_asteroid), k1r = v;
    V2 tp = {p.x+0.5*dt*k1r.x, p.y+0.5*dt*k1r.y};
    V2 tv = {v.x+0.5*dt*k1v.x, v.y+0.5*dt*k1v.y};

    V2 k2v = accel(tp, sat_pos, is_asteroid), k2r = tv;
    tp.x = p.x+0.5*dt*k2r.x; tp.y = p.y+0.5*dt*k2r.y;
    tv.x = v.x+0.5*dt*k2v.x; tv.y = v.y+0.5*dt*k2v.y;

    V2 k3v = accel(tp, sat_pos, is_asteroid), k3r = tv;
    tp.x = p.x+dt*k3r.x; tp.y = p.y+dt*k3r.y;
    tv.x = v.x+dt*k3v.x; tv.y = v.y+dt*k3v.y;

    V2 k4v = accel(tp, sat_pos, is_asteroid), k4r = tv;

    b->pos.x = p.x + (dt/6.0)*(k1r.x + 2*k2r.x + 2*k3r.x + k4r.x);
    b->pos.y = p.y + (dt/6.0)*(k1r.y + 2*k2r.y + 2*k3r.y + k4r.y);
    b->vel.x = v.x + (dt/6.0)*(k1v.x + 2*k2v.x + 2*k3v.x + k4v.x);
    b->vel.y = v.y + (dt/6.0)*(k1v.y + 2*k2v.y + 2*k3v.y + k4v.y);
}

/* ── Utilitaires rendu ───────────────────────────────────────────────────── */
static inline int wx(double x) { return (int)(WIN_W/2 + x * SCALE); }
static inline int wy(double y) { return (int)(WIN_H/2 - y * SCALE); }

static void fill_circle(SDL_Renderer *r, int cx, int cy, int rad) {
    for (int dy = -rad; dy <= rad; dy++)
        for (int dx = -rad; dx <= rad; dx++)
            if (dx*dx + dy*dy <= rad*rad)
                SDL_RenderDrawPoint(r, cx+dx, cy+dy);
}

static void trail_push(Body *b) {
    b->trail[b->trail_head] = b->pos;
    b->trail_head = (b->trail_head + 1) % TRAIL_LEN;
    if (b->trail_count < TRAIL_LEN) b->trail_count++;
}

static void trail_draw(SDL_Renderer *rend, Body *b) {
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < b->trail_count; i++) {
        int idx   = (b->trail_head - b->trail_count + i + TRAIL_LEN) % TRAIL_LEN;
        int alpha = (int)(210.0 * i / b->trail_count);
        SDL_SetRenderDrawColor(rend, b->r, b->g, b->b, (Uint8)alpha);
        SDL_RenderDrawPoint(rend, wx(b->trail[idx].x), wy(b->trail[idx].y));
    }
}

/* ─────────────────────────────────────────────────────────────────────────── */
int main(void) {
    srand((unsigned)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow(
        "Intrusion d'un satellite dans la ceinture de Saturne",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* ── Satellite : orbite elliptique ──
     *  apoapsis  ra = 350 000 km  (départ, hors ceinture)
     *  périapsis rp =  80 000 km  (plonge dans la ceinture)
     *  demi-grand axe a = 215 000 km
     *  v_apo = sqrt(G*MS*(2/ra - 1/a)) ≈ 6.33 km/s
     */
    double ra = 350000.0, rp = 80000.0;
    double a_orb = (ra + rp) / 2.0;
    double e     = (ra - rp) / (ra + rp);
    double gms   = G * MS;
    double v_apo = sqrt(gms * (2.0/ra - 1.0/a_orb));

    Body satellite = {0};
    satellite.pos  = (V2){ra, 0.0};
    satellite.vel  = (V2){0.0, v_apo};
    satellite.r = 210; satellite.g = 50; satellite.b = 50;   /* rouge vif   */

    /* ── 30 astéroïdes en ceinture circulaire 100 000 – 230 000 km ── */
    Body asteroids[NUM_ASTEROIDS] = {0};
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        double dist  = 100000.0 + i * (130000.0 / (NUM_ASTEROIDS - 1));
        double angle = ((double)i / NUM_ASTEROIDS) * 2.0 * M_PI
                       + ((double)rand()/RAND_MAX) * 0.25;
        double speed = sqrt(gms / dist);
        asteroids[i].pos = (V2){dist*cos(angle), dist*sin(angle)};
        asteroids[i].vel = (V2){-speed*sin(angle), speed*cos(angle)};

        /* couleur : bleu clair → violet selon distance */
        double t = (double)i / (NUM_ASTEROIDS - 1);
        asteroids[i].r = (Uint8)(40  + t * 160);
        asteroids[i].g = (Uint8)(80  - t * 40);
        asteroids[i].b = (Uint8)(210 - t * 70);
    }

    int running = 1, frame = 0;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
                running = 0;
        }

        /* Physique */
        for (int s = 0; s < STEPS_PER_FRAME; s++) {
            V2 sp = satellite.pos;
            for (int i = 0; i < NUM_ASTEROIDS; i++)
                rk4(&asteroids[i], sp, DT, 1);
            rk4(&satellite, (V2){0, 0}, DT, 0);
        }

        /* Traînées */
        if (frame % 2 == 0) {
            trail_push(&satellite);
            for (int i = 0; i < NUM_ASTEROIDS; i++)
                trail_push(&asteroids[i]);
        }
        frame++;

        /* ── Rendu ── */
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderClear(rend);

        /* Orbite théorique du satellite (pointillés rouges transparents) */
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rend, 210, 50, 50, 60);
        for (int i = 0; i < 720; i += 2) {
            double ang   = i * M_PI / 360.0;
            double r_ell = a_orb * (1.0 - e*e) / (1.0 + e * cos(ang));
            SDL_RenderDrawPoint(rend, wx(r_ell*cos(ang)), wy(r_ell*sin(ang)));
        }

        /* Traînées */
        trail_draw(rend, &satellite);
        for (int i = 0; i < NUM_ASTEROIDS; i++)
            trail_draw(rend, &asteroids[i]);

        /* Saturne */
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
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

        SDL_RenderPresent(rend);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
