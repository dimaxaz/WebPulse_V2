from fastapi import HTTPException
import pyotp
import qrcode
import io
import base64
from typing import Tuple
from ..models.auth import User
import redis
from ..config import settings

class TwoFactorAuth:
    def __init__(self):
        self.redis = redis.Redis(
            host=settings.REDIS_HOST,
            port=settings.REDIS_PORT,
            db=settings.REDIS_DB_2FA
        )
    
    def generate_secret(self, user: User) -> Tuple[str, str]:
        """Генерирует секрет и QR-код для 2FA"""
        secret = pyotp.random_base32()
        totp = pyotp.TOTP(secret)
        
        # Создаем QR-код
        provisioning_uri = totp.provisioning_uri(
            user.email,
            issuer_name=settings.APP_NAME
        )
        
        qr = qrcode.QRCode(version=1, box_size=10, border=5)
        qr.add_data(provisioning_uri)
        qr.make(fit=True)
        
        img = qr.make_image(fill_color="black", back_color="white")
        buffered = io.BytesIO()
        img.save(buffered, format="PNG")
        qr_code = base64.b64encode(buffered.getvalue()).decode()
        
        # Сохраняем секрет во временное хранилище
        self.redis.setex(
            f"2fa_setup:{user.id}",
            300,  # 5 минут на подтверждение
            secret
        )
        
        return secret, qr_code
    
    def verify_setup(self, user_id: int, code: str) -> bool:
        """Проверяет код при настройке 2FA"""
        secret = self.redis.get(f"2fa_setup:{user_id}")
        if not secret:
            raise HTTPException(
                status_code=400,
                detail="2FA setup expired"
            )
        
        secret = secret.decode()
        totp = pyotp.TOTP(secret)
        
        if totp.verify(code):
            # Сохраняем подтвержденный секрет
            self.redis.set(f"2fa:{user_id}", secret)
            self.redis.delete(f"2fa_setup:{user_id}")
            return True
        return False
    
    def verify_code(self, user_id: int, code: str) -> bool:
        """Проверяет код при входе"""
        secret = self.redis.get(f"2fa:{user_id}")
        if not secret:
            raise HTTPException(
                status_code=400,
                detail="2FA not setup"
            )
        
        secret = secret.decode()
        totp = pyotp.TOTP(secret)
        return totp.verify(code)

two_factor_auth = TwoFactorAuth() 