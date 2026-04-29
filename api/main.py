"""
main.py
Point d'entrée de l'API FastAPI pour le portail BI UPES.
"""

import os
from contextlib import asynccontextmanager

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

from models import Base
from routers import etudiants, enseignants, finances, recherche, kpis, auth

DATABASE_URL = os.getenv(
    "DATABASE_URL",
    "postgresql+psycopg2://upes:upes_password@localhost:5432/upes_dwh",
)

engine = create_engine(DATABASE_URL, pool_pre_ping=True, echo=False)
SessionLocal = sessionmaker(bind=engine, autocommit=False, autoflush=False)


@asynccontextmanager
async def lifespan(app: FastAPI):
    # Créer les tables si elles n'existent pas
    Base.metadata.create_all(bind=engine)
    yield


app = FastAPI(
    title="UPES BI Portal API",
    description="API REST pour le portail Business Intelligence de l'UPES",
    version="1.0.0",
    lifespan=lifespan,
)

# ── CORS ──────────────────────────────────────────────────────────────────────
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000", "http://localhost:5173"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ── Routers ───────────────────────────────────────────────────────────────────
app.include_router(auth.router,        prefix="/api/auth",        tags=["Authentification"])
app.include_router(etudiants.router,   prefix="/api/etudiants",   tags=["Étudiants"])
app.include_router(enseignants.router, prefix="/api/enseignants", tags=["Enseignants"])
app.include_router(finances.router,    prefix="/api/finances",    tags=["Finances"])
app.include_router(recherche.router,   prefix="/api/recherche",   tags=["Recherche"])
app.include_router(kpis.router,        prefix="/api/kpis",        tags=["KPIs"])


@app.get("/", tags=["Santé"])
def root():
    return {"status": "ok", "message": "UPES BI API opérationnelle"}


@app.get("/health", tags=["Santé"])
def health():
    return {"status": "healthy"}
