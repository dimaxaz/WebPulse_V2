from fastapi import APIRouter, Depends, HTTPException
from ..security.api_keys import api_key_manager, get_api_key
from ..security.two_factor import two_factor_auth
from ..auth.jwt import get_current_user
from ..models.auth import User
from typing import List

router = APIRouter()

@router.post("/api-keys/generate")
async def generate_api_key(
    scopes: List[str],
    current_user: User = Depends(get_current_user)
):
    api_key = api_key_manager.generate_api_key(
        current_user.id,
        scopes
    )
    return {"api_key": api_key}

@router.post("/api-keys/revoke")
async def revoke_api_key(
    api_key: str,
    current_user: User = Depends(get_current_user)
):
    key_data = api_key_manager.validate_api_key(api_key)
    if key_data and key_data["user_id"] == current_user.id:
        api_key_manager.revoke_api_key(api_key)
        return {"message": "API key revoked"}
    raise HTTPException(
        status_code=403,
        detail="Invalid API key"
    )

@router.post("/2fa/setup")
async def setup_2fa(
    current_user: User = Depends(get_current_user)
):
    secret, qr_code = two_factor_auth.generate_secret(current_user)
    return {
        "secret": secret,
        "qr_code": qr_code
    }

@router.post("/2fa/verify-setup")
async def verify_2fa_setup(
    code: str,
    current_user: User = Depends(get_current_user)
):
    if two_factor_auth.verify_setup(current_user.id, code):
        return {"message": "2FA setup successful"}
    raise HTTPException(
        status_code=400,
        detail="Invalid verification code"
    )

@router.post("/2fa/verify")
async def verify_2fa(
    code: str,
    current_user: User = Depends(get_current_user)
):
    if two_factor_auth.verify_code(current_user.id, code):
        return {"message": "2FA verification successful"}
    raise HTTPException(
        status_code=400,
        detail="Invalid verification code"
    ) 